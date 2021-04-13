/****************************************************************************
 *  @file     monitorthread.h
 *  @brief    相机设备监视线程类，监视相机是否掉线
 *  @author   junjie.zeng
 *  @email    junjie.zeng@smartmore.com
 *  @version
 *  @date     2020.12.29
 *  Copyright (c) 2020 SmartMore All rights reserved.
 ****************************************************************************/
#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#include <QMutex>
#include <QThread>
#include <unordered_map>

namespace vtk::cameraservice
{

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    MonitorThread();
    ~MonitorThread();

    void setCameraIdList(std::vector<std::string> list);
    void stopMonitor();

signals:
    void signalCameraDisConnected(std::string camera_id);

protected:
    void run() override;

private:
    bool isConnected(const QString &ip);

private:
    bool m_stop{false};
    QMutex m_mutex;
    std::vector<std::string> m_camera_id_list;
};
}  // namespace vtk::cameraservice
#endif  // MONITORTHREAD_H
