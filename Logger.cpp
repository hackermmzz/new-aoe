///  基于
///  https://github.com/VelazcoJD/QtLogging
///  进行修改

#include "Logger.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QHash>
#include <QElapsedTimer>
#include <QObject>
#include <QDebug>
#include "GlobalVariate.h"

QFile* Logger::logFile = Q_NULLPTR;
bool Logger::isInit = false;
QHash<QtMsgType, QString> Logger::contextNames = {
    {QtMsgType::QtDebugMsg,     " Debug  "},
    {QtMsgType::QtInfoMsg,      "  Info  "},
    {QtMsgType::QtWarningMsg,   "Warning "},
    {QtMsgType::QtCriticalMsg,  "Critical"},
    {QtMsgType::QtFatalMsg,     " Fatal  "}
};

Logger::LogLevel Logger::currentLogLevel = Logger::LogLevel::Debug;
QElapsedTimer Logger::elapsedTimer;  // 计时器
QStringList Logger::excludedFiles;  // 初始化排除文件列表

void Logger::init(LogLevel level) {
    if (isInit) {
        return;
    }

    // Set the log level
    currentLogLevel = level;

    // Start the elapsed timer
    elapsedTimer.start();

    // Create log file
    logFile = new QFile;
    logFile->setFileName("./GameLog.log");
    logFile->open(QIODevice::Append | QIODevice::Text);

    // Redirect logs to messageOutput
    qInstallMessageHandler(Logger::messageOutput);

    // Clear file contents
    logFile->resize(0);

    Logger::isInit = true;
    qInfo() << "日志服务启动！日志级别：" + contextNames.value((static_cast<QtMsgType>(level)));
}

void Logger::clean() {
    if (logFile != Q_NULLPTR) {
        logFile->close();
        delete logFile;
    }
}

// 添加排除文件
void Logger::addExcludedFile(const QString& filePath) {
    excludedFiles.append(filePath);
}

// 清空排除文件列表
void Logger::clearExcludedFiles() {
    excludedFiles.clear();
}

// 检查文件是否在排除列表中
bool Logger::isFileExcluded(const QString& filePath) {
    for (const QString& excludedFile : excludedFiles) {
        if (filePath.endsWith(excludedFile)) {
            return true;
        }
    }
    return false;
}

void Logger::messageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    // 检查文件是否在排除列表中
    QString currentFile = QString(context.file);
    if (isFileExcluded(currentFile)) {
        // 该文件在排除列表中，使用默认处理方式
        fprintf(stdout, "%s\n", msg.toLocal8Bit().constData());
        return;
    }

    // Only log messages with the specified log level or higher
    if (static_cast<LogLevel>(type) < currentLogLevel) {
        return;
    }

    // Get the elapsed time in milliseconds
    qint64 elapsedTime = elapsedTimer.elapsed();

    // Convert elapsed time to minutes, seconds, and milliseconds
    int minutes = static_cast<int>(elapsedTime / 60000);  // 60,000 ms = 1 minute
    int seconds = static_cast<int>((elapsedTime % 60000) / 1000);  // 1,000 ms = 1 second
    int milliseconds = static_cast<int>(elapsedTime % 1000);  // Remaining milliseconds

    // Format the elapsed time as "minutes:seconds:milliseconds"
    QString timeString = QString("%1:%2:%3")
        .arg(minutes, 2, 10, QChar('0'))  // Ensure two digits for minutes
        .arg(seconds, 2, 10, QChar('0'))  // Ensure two digits for seconds
        .arg(milliseconds, 3, 10, QChar('0'));  // Ensure three digits for milliseconds

    QString log = QObject::tr("%1 | %2 | %3 | %4 | %5 | %6 | %7\n").
        arg(timeString).                           // 使用分钟:秒:毫秒格式
        arg(g_frame).                             // 当前帧数
        arg(Logger::contextNames.value(type)).
        arg(context.line).
        arg(QString(context.file).
            section('\\', -1)).            // File name without file path
        arg(QString(context.function).
            section('(', -2, -2).        // Function name only
            section(' ', -1).
            section(':', -1)).
        arg(msg);

    logFile->write(log.toLocal8Bit());
    logFile->flush();
}
