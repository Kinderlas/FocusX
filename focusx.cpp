#include "focusx.h"
#include <QtWidgets>
#include <QSql>
#include <QDebug>

FocusX::FocusX(QWidget *parent)
        : QWidget(parent)
{
    lb_now = new QLabel("FocusX:");
    lb_start = new QLabel("start at:");
    lb_focus_time = new QLabel("working time ");

    current_id = -1;
    le_now = new QLineEdit("nothing to do");
    le_add_task = new QLineEdit();
    le_start_time = new QLineEdit();
    le_working_time = new QLineEdit();
    le_now->setReadOnly(true);

    pb_done = new QPushButton("done");
    pb_pause = new QPushButton("pause");
    pb_add_task = new QPushButton("add");

    dte_start = new QDateTimeEdit(QDateTime::currentDateTime());

    te_working_time = new QTimeEdit();
    le_working_time->setMinimumSize(100, 20);
    //le_working_time->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    le_now->setMinimumSize(100, 20);
    //le_now->setSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed);
    //le_now->setSizePolicy (QSizePolicy::Ignored, QSizePolicy::Ignored);

    cb_pin = new QCheckBox("pin", this);
    cb_simple = new QCheckBox("simplify", this);
    connect(cb_pin, SIGNAL(clicked(bool)),
            this, SLOT(set_flag_pin()));
    connect(cb_simple, SIGNAL(clicked(bool)),
            this, SLOT(set_window_simple()));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(timer_add_one_second()));
    tw_todolist = new QTreeWidget;
    tw_todolist->setColumnCount(2);
    QStringList tw_headers;
    tw_headers << "task" << "working time" << "status";
    tw_todolist->setHeaderLabels(tw_headers);
    QTreeWidgetItem* tw_root = new QTreeWidgetItem(tw_todolist, QStringList(QString("root")));
    new QTreeWidgetItem(tw_root, QStringList(QString("test1")));
    new QTreeWidgetItem(tw_root, QStringList(QString("test2")));
    QStringList temp;
    temp << "task" << "deadline";
    new QTreeWidgetItem(tw_root, temp);
    QList<QTreeWidgetItem*> rootlist;
    rootlist << tw_root;
    tw_todolist->insertTopLevelItems(0, rootlist);
    tw_todolist->setMinimumHeight(200);

    //init_db(QString("focusx.db"));
    init_db(QString("focusx.db"));
    connect(pb_add_task, SIGNAL(clicked(bool)),
            this, SLOT(insert_task()));
    connect(pb_pause, SIGNAL(clicked(bool)),
            this, SLOT(pause_task()));
    connect(pb_done, SIGNAL(clicked(bool)),
            this, SLOT(finish_task()));


    mainlayout = new QGridLayout;
    set_ori_layout();
    //mainlayout = new QGridLayout;
    setLayout(mainlayout);
    /*(mainlayout->addWidget(lb_add_task, 4, 0);
    mainlayout->addWidget(le_add_task, 4, 1);
    */
    setWindowTitle("FocusX");
    //resize(300, 300);
    update_from_db();
    tw_todolist->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tw_todolist, &QTreeWidget::customContextMenuRequested,
            this, &FocusX::right_click_menu);
    //|Qt::WindowTitleHint|Qt::Window);//| Qt::FramelessWindowHint);
    QSettings settings("kinderlas", "focusx");
    restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
    wflag = this->windowFlags();
    oflag = this->windowFlags();
    cb_pin->setChecked(true);
    //cb_simple->setChecked(true);
    wflag |= Qt::ToolTip;
    wflag |= Qt::WindowStaysOnTopHint;
    this->setWindowFlags(wflag);
    //this->setWindowFlags(wflag | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    //| Qt::ToolTip);// | Qt::WindowTitleHint );
    this->adjustSize();
    qDebug() << this->width() << ", " << this->height();
}

void FocusX::set_ori_layout(){
    mainlayout->addWidget(lb_now, 0, 0);
    mainlayout->addWidget(le_now, 0, 1);
    mainlayout->addWidget(pb_done, 0, 2);
    mainlayout->addWidget(lb_start, 1, 0);
    mainlayout->addWidget(le_start_time, 1, 1);
    mainlayout->addWidget(pb_pause, 1, 2);
    mainlayout->addWidget(lb_focus_time, 2, 0);
    mainlayout->addWidget(le_working_time, 2, 1);
    mainlayout->addWidget(tw_todolist, 3, 0, 1, 3);
    mainlayout->addWidget(pb_add_task, 2, 2);
    mainlayout->addWidget(cb_pin, 4, 2);
    mainlayout->addWidget(cb_simple, 4, 1);
    //mainlayout->SetFixedSize(200, 300);
}

void FocusX::set_flag_pin(){
    if(cb_pin->isChecked())
    {
        wflag |= Qt::ToolTip;
        wflag |= Qt::WindowStaysOnTopHint;
    }
    else
        wflag = oflag;
    this->setWindowFlags(wflag | Qt::MSWindowsFixedSizeDialogHint );
    show();
    qDebug() << this->width() << ", " << this->height();
}

void FocusX::set_window_simple(){
    QLayoutItem* it;
    while(it = mainlayout->takeAt(0))
    {
        mainlayout->removeItem(it);
        it->widget()->setParent(NULL);
    }
    //delete this->layout();
    if(cb_simple->isChecked())
    {
        //mainlayout = new QGridLayout;
        //mainlayout->addWidget(lb_focus_time, 0, 0);
        mainlayout->addWidget(le_now, 0, 0);
        mainlayout->addWidget(le_working_time, 1, 0);
        mainlayout->addWidget(cb_pin, 0, 1);
        mainlayout->addWidget(cb_simple, 1, 1);
        //mainlayout->SetFixedSize(50, 50);
        //mainlayout->setSizeConstraint(QLayout::SetMinimumSize);
        //mainlayout->setSizeConstraint(QSizePolicy::MinimumExpanding);
        //this->setMinimumSize(150, 50);
        //this->adjustSize();
    }
    else
    {
        //QSettings settings("kinderlas", "focusx");
        //restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
        set_ori_layout();
        //mainlayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
        //mainlayout->setSizeConstraint(QSizePolicy::Maximum);
        //this->setMinimumSize(300,300);
        //this->adjustSize();
    }
    //setLayout(mainlayout);
    adjustSize();
    show();
    //setMinimumSize(100, 80);
    qDebug() << sizeHint();
    qDebug() << this->width() << ", " << this->height();
}

void FocusX::closeEvent(QCloseEvent *event){
    qDebug() << "close" ;
    QSettings settings("kinderlas", "focusx");
    settings.setValue("geometry", saveGeometry());
    event->accept();
    db.close();
    QApplication::quit();
}

FocusX::~FocusX()
{
}

QString FocusX::seconds_format(int totaltime){
    int s = totaltime % 60;
    totaltime /= 60;
    int m = totaltime % 60;
    totaltime /=60;
    int h = totaltime % 24;
    int d = totaltime /= 24;
    QString ret;
    if(h == 0 && d == 0)
        return ret.sprintf("%02d m %02d s", m, s);
    if(d == 0)
        return ret.sprintf("%02d h %02d m %02d s", h, m, s);
    return ret.sprintf("%02d d %02d h %02d m %02d s",d, h, m, s);
}

bool FocusX::has_current_id(){
    return current_id != -1;
}

void FocusX::finish_task(){
    if(!has_current_id())
        return;
    bool isOK;
    QString task_infomation = QInputDialog::getMultiLineText(this, "add task",
                                                             "please add task",
                                                             "info",
                                                             &isOK);
    if(!isOK) return;
    QString sql = "update focusx set status = 'done', finished_time = '%1', result = '%2' where id = '%3'";
    sql = sql.arg(QDateTime::currentDateTime().toString("MM-dd hh:mm")).arg(task_infomation).arg(current_id);
    QSqlQuery query;
    query.exec(sql);
    current_id = -1;
    le_now->setText("nothing to do ");
    le_start_time->setText("");
    le_working_time->setText("0");
    timer->stop();
    update_from_db();
}

void FocusX::pause_task(){
    if(!has_current_id())
        return;
    if(timer->isActive())
        timer->stop();
    else
        timer->start();
}

void FocusX::timer_add_one_second(){
    QString sql = "update focusx set working_time = working_time +1 where id = '%1'";
    sql = sql.arg(current_id);
    QSqlQuery query;
    query.exec(sql);
    sql = "select working_time from focusx where id = %1";
    sql = sql.arg(current_id);
    query.exec(sql);
    sql = "0";
    if(query.next())
        sql = query.value(0).toString();
    le_working_time->setText(seconds_format(sql.toInt()));
}

bool FocusX::edit_task(int id){
    QString sql = "select * from focusx where id = '%1'";
    sql = sql.arg(id);
    QSqlQuery query;
    query.exec(sql);
    if(!query.next())
        return false;
    bool isOK;
    QString new_task_name = QInputDialog::getMultiLineText(this, "modify task",
                                                           "please enter new task",
                                                           "name",
                                                           &isOK);
    if(isOK)
    {
        sql = "update focusx set content = '%1' where id = '%2'";
        sql = sql.arg(new_task_name).arg(id);
        query.exec(sql);
        update_from_db();
    }
    return true;
}

bool FocusX::start_task(int id){
    current_id = id;
    qDebug() << "start task " << QString::number(id);
    QString sql = "select * from focusx where id = '%1' and status = 'todo'";
    sql = sql.arg(id);
    QSqlQuery query;
    query.exec(sql);
    if(!query.next())
        return false;
    le_now->setText(query.value("content").toString());
    QString start_time;
    if(query.value("start_time").toString().length() == 0){
        sql = "update focusx set start_time = '%1' where id = '%2'";
        start_time = QDateTime::currentDateTime().toString("MM-dd hh:mm");
        sql = sql.arg(start_time).arg(id);
        QSqlQuery tmp;
        tmp.exec(sql);
    }
    else
        start_time = query.value("start_time").toString();
    le_start_time->setText(start_time);
    timer->start(1000);
    return true;
}

bool FocusX::delete_task(int id){
    qDebug() << "delete task " << QString::number(id);
    QString sql = "delete from focusx where id = '%1'";
    sql = sql.arg(id);
    QSqlQuery query;
    bool ret = query.exec(sql);
    update_from_db();
    return ret;
}

bool FocusX::add_sub_task(QString task_name){
    QString sql = "select id from focusx where content = '%1' and status = 'todo' and father_task = '0' order by id desc";
    sql = sql.arg(task_name);
    QSqlQuery query;
    query.exec(sql);
    if(!query.next())
        return false;
    bool isOK;
    QString sub_task_string = QInputDialog::getMultiLineText(this, "add task",
                                                             "please add task",
                                                             "new task",
                                                             &isOK);
    if(isOK)
    {
        sql = "insert into focusx (content, status, father_task, working_time) "
              "values('%1','todo','%2','0')";
        sql = sql.arg(sub_task_string).arg(query.value(0).toInt());
        query.exec(sql);
        update_from_db();
    }
    return true;
}

bool FocusX::delete_subtask(const QString & str){

    qDebug() << "delete start" << str;
    QString sql = "select id from focusx where content = '%1' and father_task = '0' and status = 'todo' order by id desc";
    sql = sql.arg(str);
    QSqlQuery query;
    query.exec(sql);
    if(!query.next())
        return false;
    int f_id = query.value(0).toInt();
    sql = QString("delete from focusx where father_task = '%1'").arg(f_id);
    query.exec(sql);
    sql = QString("delete from focusx where id = '%1'").arg(f_id);
    query.exec(sql);
    update_from_db();
    return true;
}

void FocusX::right_click_menu(const QPoint &pos){
    QTreeWidgetItem *it = 0;
    it = tw_todolist->itemAt(pos);
    if(!it)
        return;
    if(it->text(3) == "")
    {

        QSignalMapper* map = new QSignalMapper(this);
        QSignalMapper* map_delete_all = new QSignalMapper(this);
        QAction *act = new QAction("add_sub_task", this);
        QAction *act_delte_all = new QAction("delete task set", this);
        connect(act, SIGNAL(triggered()),
                map, SLOT(map()));
        connect(act_delte_all, SIGNAL(triggered()),
                map_delete_all, SLOT(map()));
        map->setMapping(act, it->text(0));
        map_delete_all->setMapping(act_delte_all, it->text(0));
        connect(map, SIGNAL(mapped(QString)),
                this, SLOT(add_sub_task(QString)));
        connect(map_delete_all, SIGNAL(mapped(QString)),
                this, SLOT(delete_subtask(QString)));
        QMenu menu(this);
        menu.addAction(act);
        menu.addAction(act_delte_all);
        QPoint pt(pos);
        menu.exec(tw_todolist->mapToGlobal(pt));
        return;
    }

    QSignalMapper* map = new QSignalMapper(this);
    QSignalMapper* map_delete = new QSignalMapper(this);
    QSignalMapper* map_edit = new QSignalMapper(this);

    QAction *act = new QAction("start", this);
    connect(act, SIGNAL(triggered()),
            map, SLOT(map()));
    map->setMapping(act, it->text(3).toInt());
    connect(map, SIGNAL(mapped(int)),
            this, SLOT(start_task(int)));

    QAction *act_delete = new QAction("delete", this);
    connect(act_delete, SIGNAL(triggered()),
            map_delete, SLOT(map()));
    map_delete->setMapping(act_delete, it->text(3).toInt());
    connect(map_delete, SIGNAL(mapped(int)),
            this, SLOT(delete_task(int)));

    QAction *act_edit = new QAction("edit", this);
    connect(act_edit, SIGNAL(triggered()),
            map_edit, SLOT(map()));
    map_edit->setMapping(act_edit, it->text(3).toInt());
    connect(map_edit, SIGNAL(mapped(int)),
            this, SLOT(edit_task(int)));

    QMenu menu(this);
    menu.addAction(act);
    menu.addAction(act_delete);
    menu.addAction(act_edit);
    QPoint pt(pos);
    menu.exec(tw_todolist->mapToGlobal(pt));
}

bool FocusX::init_db(const QString& db_name){
    db = QSqlDatabase::addDatabase("QSQLITE");
#ifdef Q_OS_LINUX
    QString path(QDir::home().path());
    path.append(QDir::separator()).append(".focusx");
    QDir dir;
    dir.mkpath(path);
    path.append(QDir::separator()).append(db_name);
    path = QDir::toNativeSeparators(path);
    qDebug() << path;
    db.setDatabaseName(path);
#else
    // NOTE: File exists in the application private folder, in Symbian Qt implementation
    db.setDatabaseName(db_name);
#endif
    if(!db.open()){
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());
        return false;
    }
    if(!db.tables().contains("focusx")){
        QSqlQuery query;
        if(!query.exec("create table focusx ("
                       "id integer primary key autoincrement,"
                       "content varchar(50),"
                       "start_time varchar(30),"
                       "working_time integer,"
                       "deadline varchar(30),"
                       "finished_time varchar(30),"
                       "result varchar(300),"
                       "status varchar(10),"
                       "father_task integer)")){
            QMessageBox::critical(0, QObject::tr("Database Error"), db.lastError().text());
            return false;
        }
    }
    return true;
}

int FocusX::db_add_task(const QString& task_name, int father_id){
    QSqlQuery query;
    QString sql = "insert into focusx (content, status, father_task, working_time) "
                  "values('" + task_name +"','todo','" + QString::number(father_id) +"','0')";
    bool t = query.exec(sql);
    qDebug() << t;
    qDebug() << sql;
    return query.lastInsertId().toInt();
}

bool FocusX::update_working_time(){
    QSqlQuery query, qq;
    query.exec("select * from focusx where father_task = 0");
    while(query.next()){
        int total = 0;
        qq.exec("select * from focusx where father_task = " + query.value(0).toString());
        while(qq.next()){
            total += qq.value("working_time").toInt();
        }
        QString sql = QString("update focusx set working_time = '%1' where id = '%2'")
                        .arg(total).arg(query.value(0).toString());
        qDebug() << "update working time" << sql;
        query.exec(sql);
    }
    return true;
}

bool FocusX::update_from_db(){
    QSqlQuery query, qq;
    query.exec("select * from focusx where father_task = 0 ");
    tw_todolist->clear();
    while(query.next()){
        QStringList tmp;
        tmp << query.value("content").toString() << query.value("working_time").toString() << query.value("id").toString();
        //QTreeWidgetItem* tw_root = new QTreeWidgetItem(tw_todolist, QStringList(query.value("content").toString()));
        QTreeWidgetItem* tw_root = new QTreeWidgetItem(QStringList(query.value("content").toString()));
        QString slt = "select * from focusx where father_task = %1";// and status = 'todo'";
        slt = slt.arg(query.value(0).toString());
        qq.exec(slt);
        bool vis = false;
        while(qq.next()){
            vis = true;
            QStringList t;
            t << qq.value("content").toString() << seconds_format(qq.value("working_time").toInt())
              << qq.value("status").toString()<< qq.value("id").toString();
            tw_root->addChild(new QTreeWidgetItem(t));
            //tw_root->addChild(new QTreeWidgetItem(QStringList(qq.value("content").toString())));
        }
        tw_todolist->addTopLevelItem(tw_root);
        tw_root->setExpanded(true);
        /*
    if(vis){
    tw_todolist->addTopLevelItem(tw_root);
    }
    else{
    QString sql = "delete from focusx where id = '%1'";
    sql = sql.arg(query.value(0).toString());
    qq.exec(sql);
    qDebug() << sql;
    }
    */
    }

    return true;
}

void FocusX::insert_task()
{
    bool isOK;
    QString task_string = QInputDialog::getMultiLineText(this, "add task",
                                                         "please add task",
                                                         "new task",
                                                         &isOK);
    if(isOK){
        QStringList ret = task_string.split("\n>");
        int id_of_father = db_add_task(ret.at(0), 0);
        qDebug() << "father id:" << id_of_father;
        //QTreeWidgetItem* tw_root = new QTreeWidgetItem(tw_todolist, QStringList(ret.at(0)));
        for(int i = 1; i < ret.size(); i++){
            db_add_task(ret.at(i), id_of_father);
        }
        update_from_db();
    }
}
