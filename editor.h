#ifndef EDITOR_H
#define EDITOR_H

#include <QApplication>
#include <QWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QDebug>
#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QMenu>
#include <QDockWidget>


namespace Ui {
class Editor;
}

class Editor : public QWidget
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = 0);
    ~Editor();

private:
    Ui::Editor *ui;
    QPushButton *terrainButton;
    QComboBox *heightComboBox;
    QPushButton *resourceButton;
    QMenu *terrainMenu;
    QMenu *resourceMenu;
    QDockWidget *dockWidget;
    QPushButton *showEditorButton;
signals:
    void treeSelected();  // 当树被选中时发出信号
public slots:
    void onEditorButtonClick() {
        if (isVisible()) {
            hide();
        } else {
            show();
        }
    }
    void onTreeSelected()
    {
        emit treeSelected();  // 发出信号
    }
};

#endif // EDITOR_H
