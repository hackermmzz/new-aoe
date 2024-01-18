#ifndef ACTWIDGET_H
#define ACTWIDGET_H

#include <QWidget>
#include <QPainter>
#include <config.h>
#include <Player.h>
#include <Map.h>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QDebug>
class ActWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ActWidget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setNum(int num);
    void setPhase(int phase);
    void setStatus(int status);
    void setPix(QPixmap pix);
    void setActName(int actName);
    int getNum();
    int getPhase();
    int getStatus();
    QPixmap getPix();
    int getActName();
private:
    int num;
    int phase;
    int status = 0;
    QPixmap pix;
    int actName;
signals:
    void actPress(int num);
public slots:
};

#endif // ACTWIDGET_H
