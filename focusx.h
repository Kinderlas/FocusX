#ifndef FOCUSX_H
#define FOCUSX_H
#include <QtWidgets>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QDateTime>
#include <QDir>

class FocusX : public QWidget
{
    Q_OBJECT

public:
    FocusX(QWidget *parent = 0);
    ~FocusX();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Qt::WindowFlags wflag;
    Qt::WindowFlags oflag;
    QTreeWidget* tw_todolist;
    QLineEdit* le_now ;
    QLineEdit* le_add_task;
    QLineEdit* le_start_time;
    QLineEdit* le_working_time;

    QLabel* lb_now;
    QLabel* lb_start;
    QLabel* lb_focus_time;
    QPushButton* pb_done;
    QPushButton* pb_pause;
    QPushButton* pb_add_task;

    QDateTimeEdit* dte_start ;
    QTimeEdit* te_working_time ;

    QCheckBox* cb_pin;
    QCheckBox* cb_simple;

    int current_id;

    QTimer* timer;

    QSqlDatabase db;
    void create_todoTable(QTableWidget* todoTable);
    QGridLayout* mainlayout;
    //QStringList analyze_task(QString *input_string);
    bool connect_db(const QString &db_name);
    bool init_db(const QString &db_name);
    int db_add_task(const QString &task_name, int father_id);
    bool update_from_db();
    bool update_working_time();
    QString seconds_format(int s);
    bool has_current_id();
    void set_ori_layout();
private slots:
    void set_window_simple();
    void insert_task();
    void right_click_menu(const QPoint &pos);
    bool start_task(int id);
    bool delete_task(int id);
    void timer_add_one_second();
    void pause_task();
    void finish_task();
    bool add_sub_task(QString task_name);
    bool delete_subtask(const QString &str);
    void set_flag_pin();
    bool edit_task(int id);
};

#endif // FOCUSX_H
