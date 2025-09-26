#ifndef LOGGER_H
#define LOGGER_H

///  基于
///  https://github.com/VelazcoJD/QtLogging
///  进行修改

#include <QMessageLogContext>
#include <QFile>
#include <QHash>
#include <QElapsedTimer>
#include <QStringList>

class Logger {
public:
    enum LogLevel {
        Debug = QtMsgType::QtDebugMsg,
        Info = QtMsgType::QtInfoMsg,
        Warning = QtMsgType::QtWarningMsg,
        Critical = QtMsgType::QtCriticalMsg,
        Fatal = QtMsgType::QtFatalMsg
    };

    static void init(LogLevel level = LogLevel::Debug);
    static void clean();
    static void messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    // 添加排除文件功能
    static void addExcludedFile(const QString& filePath);
    static void clearExcludedFiles();
    static bool isFileExcluded(const QString& filePath);

private:
    static QFile* logFile;
    static bool isInit;
    static LogLevel currentLogLevel;
    static QElapsedTimer elapsedTimer;  // 记录启动时间
    static QHash<QtMsgType, QString> contextNames;
    static QStringList excludedFiles;  // 存储排除的文件列表
};

#endif // LOGGER_H
