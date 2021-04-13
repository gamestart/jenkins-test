/****************************************************************************
 *  @file     asyncimagesaver.cpp
 *  @brief    异步图像保存类
 *  @author   xiaoyu.liu
 *  @email    xiaoyu.liu@smartmore.com
 *  @version
 *  @date     2020-12-22
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/

#include "asyncimagesaver.h"
#include "common/applogger.h"
#include "configdatamanager/configmanager.h"
#include <QDateTime>
#include <QDir>
#include <QMutex>
#include <QMutexLocker>
#include <QStorageInfo>
#include <QTextCodec>
#include <QThread>
#include <QVector>
#include <QWaitCondition>

namespace vtk::imagesave
{

#define OKFOLDER "/OK"
#define NGFOLDER "/NG"
#define AIOKFOLDER "/OK-AI"
#define AINGFOLDER "/NG-AI"

using Message = vtk::common::Message;
using ConfigManager = vtk::config::ConfigManager;

template <class T>

class SafeQueue : public QList<T>
{
public:
    inline void enqueue(const T &t)
    {
        QMutexLocker lock(&m_mutex);
        QList<T>::append(t);
    }

    inline T dequeue()
    {
        QMutexLocker lock(&m_mutex);
        return QList<T>::takeFirst();
    }

    inline bool isEmpty()
    {
        QMutexLocker lock(&m_mutex);
        return QList<T>::isEmpty();
    }

    inline void clear()
    {
        QMutexLocker lock(&m_mutex);
        QList<T>::clear();
    }

private:
    QMutex m_mutex;
};

class AsyncImageSaverImpl : public QThread
{
public:
    AsyncImageSaverImpl() = default;

    ~AsyncImageSaverImpl()
    {
        stopAllTasks();
    }

    void stopAllTasks()
    {
        if (isRunning())
        {
            QMutexLocker lock(&m_mutex);
            m_is_stop = true;
            m_condition.wakeOne();
        }
        wait();
    }

    void addTasks(const QList<ImageInfo> &image_info_list)
    {
        for (const auto &info : image_info_list)
            m_image_info_queue.enqueue(info);
        QMutexLocker lock(&m_mutex);
        m_condition.wakeOne();
    }

    bool saveImage(const QString &full_name, const cv::Mat &mat)
    {
        if (full_name.isEmpty())
            return false;

        QFileInfo fi(full_name);
        const auto &absolute_path = fi.absolutePath();
        QDir dir(absolute_path);
        if (!dir.exists())
            dir.mkpath(absolute_path);

        QStorageInfo storage_info;
        storage_info.setPath(absolute_path);
        storage_info.refresh();
        const auto free_rate =
            static_cast<float>(storage_info.bytesAvailable()) / static_cast<float>(storage_info.bytesTotal());
        if (free_rate < 0.200)
        {
            Message::postWarningMessage(QObject::tr("The disk free space is less than 20%, "
                                                    "and the image saving function has stopped. "
                                                    "Please clean up the disk in time!"));
            LOGW(AsyncImageSaverImpl, "The disk free space is less than 20%% and free rate is %f!", free_rate);
            QMutexLocker lock(&m_mutex);
            m_is_stop = true;
            return false;
        }

#ifdef Q_OS_WIN
        QTextCodec *gbk_code = QTextCodec::codecForName("GBK");
        auto name = std::string(gbk_code->fromUnicode(full_name.data()));
#else
        auto name = full_name.toStdString();
#endif
        if (!cv::imwrite(name, mat))
        {
            LOGW(AsyncImageSaverImpl, "failed to write image to file '%s'!", name.c_str());
            return false;
        }
        return true;
    }

protected:
    void run()
    {
        getAllOriginalImageFiles();
        getAllRenderedImageFiles();
        m_is_stop = false;
        while (!m_is_stop)
        {
            m_mutex.lock();
            m_condition.wait(&m_mutex);
            m_mutex.unlock();
            while (!m_image_info_queue.isEmpty() && !m_is_stop)
            {
                const auto &image_info = m_image_info_queue.dequeue();
                checkDeleteConditionOfSaveImages(image_info.is_src_image);
                if (saveImage(image_info.image_full_name, image_info.mat))
                {
                    image_info.is_src_image ? m_original_image_vec.append(image_info.image_full_name) :
                                              m_rendered_image_vec.append(image_info.image_full_name);
                }
                msleep(100);
            }
        }
    }

private:
    void checkDeleteConditionOfSaveImages(bool is_src_image)
    {
        auto check_delete_condition = [&](const vtk::config::SaveConfigItem &save_config, QVector<QString> &vec) {
            // 判断是否要删除文件
            if (save_config.delete_pic)
            {
                // 按张数删除
                auto count = vec.size();
                while (count > 0 && (count - save_config.save_number >= 0))
                {
                    const auto &file_name = vec.takeFirst();
                    if (!QFile::remove(file_name))
                        LOGW(AsyncImageSaverImpl, "Remove file '%s' failed!", file_name.toLatin1().data());
                    count = vec.size();
                }

                // 按天数删除
                auto days = save_config.save_day;
                if (isStorageDaysExceeded(save_config.ok_path + (is_src_image ? OKFOLDER : AIOKFOLDER), days) ||
                    isStorageDaysExceeded(save_config.ng_path + (is_src_image ? NGFOLDER : AINGFOLDER), days))
                {
                    deleteEarliestDir(save_config.ok_path + (is_src_image ? OKFOLDER : AIOKFOLDER));
                    deleteEarliestDir(save_config.ng_path + (is_src_image ? NGFOLDER : AINGFOLDER));
                }
            }
        };

        const auto &save_config_data = ConfigManager::getInstance().getImageSaveConfigData();
        check_delete_condition(is_src_image ? save_config_data.original : save_config_data.rendered,
                               is_src_image ? m_original_image_vec : m_rendered_image_vec);
    }

    bool isStorageDaysExceeded(const QString &path, int days)
    {
        QDir dir(path);
        if (dir.exists())
        {
            dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            dir.setSorting(QDir::Time | QDir::Reversed);

            const auto &list = dir.entryInfoList();
            if (list.size() >= 1)
            {
                const auto &file_info = list.at(0);
                const auto &time = file_info.fileTime(QFile::FileTime::FileBirthTime);
                if (time.daysTo(QDateTime::currentDateTime()) > days)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void deleteAllImageFilesOfCurrentPath(const QString &path)
    {
        QDir dir(path);
        if (dir.exists())
        {
            dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
            const auto &list = dir.entryInfoList();
            if (list.size() >= 1)
            {
                for (auto &file_info : list)
                {
                    if (file_info.isDir())
                    {
                        deleteAllImageFilesOfCurrentPath(file_info.filePath());
                    }
                    else if (file_info.isFile())
                    {
                        const auto &suffix = file_info.suffix().toLower();
                        if (suffix == "jpg" || suffix == "png" || suffix == "bmp")
                            QFile::remove(file_info.filePath());
                    }
                }
            }
            if (dir.isEmpty())
                dir.removeRecursively();
        }
    }

    void deleteEarliestDir(const QString &path)
    {
        QDir dir(path);
        if (dir.exists())
        {
            dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            const auto &list = dir.entryInfoList();

            for (const auto &file_info : list)
            {
                if (file_info.isDir())
                    deleteAllImageFilesOfCurrentPath(file_info.filePath());
            }
        }
    }

    void getAllOriginalImageFiles()
    {
        m_original_image_vec.clear();
        const auto &save_config_data = ConfigManager::getInstance().getImageSaveConfigData();
        getImageFileList(save_config_data.original.ok_path + OKFOLDER, m_original_image_vec);
        getImageFileList(save_config_data.original.ng_path + NGFOLDER, m_original_image_vec);
        std::sort(m_original_image_vec.begin(), m_original_image_vec.end());
    }

    void getAllRenderedImageFiles()
    {
        m_rendered_image_vec.clear();
        const auto &save_config_data = ConfigManager::getInstance().getImageSaveConfigData();
        getImageFileList(save_config_data.rendered.ok_path + AIOKFOLDER, m_rendered_image_vec);
        getImageFileList(save_config_data.rendered.ng_path + AINGFOLDER, m_rendered_image_vec);
        std::sort(m_rendered_image_vec.begin(), m_rendered_image_vec.end());
    }

    void getImageFileList(const QString &path, QVector<QString> &file_list)
    {
        QDir dir(path);
        if (dir.exists())
        {
            dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
            const auto &list = dir.entryInfoList();
            for (const auto &file_info : list)
            {
                if (file_info.isDir())
                    getImageFileList(file_info.filePath(), file_list);
                else if (file_info.isFile())
                {
                    const auto &suffix = file_info.suffix().toLower();
                    if (suffix == "jpg" || suffix == "png" || suffix == "bmp")
                        file_list.append(file_info.filePath());
                }
            }
        }
    }

private:
    bool m_is_stop{false};
    SafeQueue<ImageInfo> m_image_info_queue;
    QWaitCondition m_condition;
    QMutex m_mutex;
    QVector<QString> m_original_image_vec;
    QVector<QString> m_rendered_image_vec;
};

static AsyncImageSaverImpl *s_saver{nullptr};

QString getTodayFilePath(const QString &path, int view_id)
{
    return path + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "/" + QString::number(view_id);
}

void AsyncImageSaver::startTasks()
{
    if (!s_saver)
        s_saver = new AsyncImageSaverImpl;

    if (s_saver->isRunning())
        s_saver->stopAllTasks();

    s_saver->start();
}

void AsyncImageSaver::saveResultImages(int view_index, const vtk::common::AlgoResult &algo_result, bool is_local_image)
{
    const auto &save_config_data = ConfigManager::getInstance().getImageSaveConfigData();
    auto ok = algo_result.isOk();
    const auto &date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss_zzz");

    view_index += 1;
    QList<ImageInfo> image_info_list;

    auto append_image_info = [&](const QString &path, const QString &str, const QString &format, cv::Mat image,
                                 bool is_src_image) {
        auto full_path = getTodayFilePath(path, view_index);
        full_path += "/" + date_time + "-" + QString::number(view_index) + str + format;
        image_info_list.append({is_src_image, full_path, image});
    };

    if (save_config_data.original.save_pic && !algo_result.image.empty() && !is_local_image)
    {
        const auto &image_format = save_config_data.original.pic_format;
        if (save_config_data.original.save_ok && ok)
            append_image_info(save_config_data.original.ok_path + OKFOLDER, "-OK.", image_format, algo_result.image,
                              true);

        if (save_config_data.original.save_ng && !ok)
            append_image_info(save_config_data.original.ng_path + NGFOLDER, "-NG.", image_format, algo_result.image,
                              true);
    }

    if (save_config_data.rendered.save_pic && !algo_result.image.empty() && !algo_result.ai_image.empty())
    {
        const auto &image_format = save_config_data.rendered.pic_format;
        const auto ai_image = algo_result.ai_image.empty() ? algo_result.image : algo_result.ai_image;
        if (save_config_data.rendered.save_ok && ok)
            append_image_info(save_config_data.rendered.ok_path + AIOKFOLDER, "-OK_AI.", image_format, ai_image, false);

        if (save_config_data.rendered.save_ng && !ok)
            append_image_info(save_config_data.rendered.ng_path + AINGFOLDER, "-NG_AI.", image_format, ai_image, false);
    }

    if (!image_info_list.isEmpty())
        addTasks(image_info_list);
}

void AsyncImageSaver::addTasks(const QList<ImageInfo> &image_info_list)
{
    if (s_saver)
        s_saver->addTasks(image_info_list);
}

void AsyncImageSaver::waitQuit()
{
    if (s_saver)
    {
        delete s_saver;
        s_saver = nullptr;
    }
}

}  // namespace vtk::imagesave
