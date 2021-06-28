#!/usr/bin/env groovy

pipeline {
    agent any;
    environment { //环境变量
		GREETING="Hello";
    }
    stages{
        stage('打招呼') {
            steps{
            	sh 'echo "$GREETING $TITLE"'
            }
        }
   }
   post { //构建完成后置操作
        aborted { //如果构建中断，则执行
            echo '构建被中止!'
        }
        success { //构建成功执行
            echo '构建成功!'
        }
       failure { //构建失败执行
           echo '构建失败!'
       }
    }
}
