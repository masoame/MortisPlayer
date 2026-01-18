#include "holomainwindow.hpp"
#include "./ui_holomainwindow.h"
#include "nettofiledialog.hpp"
#include"HoloTitleWidget.hpp"
#include "QMessageBox"
#include "qfiledialog.h"

#include <iostream>
#include <syncstream>

constexpr const char s_stop_style[]="            \
    QPushButton:!hover{                                 \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page1.png)    \
}                                                       \
    QPushButton:hover {                                 \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page0.png);   \
}                                                       \
    QPushButton:pressed {                               \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page1.png);   \
}                                                       \
";

constexpr const char s_run_style[]="             \
    QPushButton:!hover{                                 \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page3.png)    \
}                                                       \
    QPushButton:hover {                                 \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page2.png);   \
}                                                       \
    QPushButton:pressed {                               \
        border:none;                                    \
        image:url(:/Image/icon/Image/icon/page3.png);   \
}                                                       \
";

HoloMainWindow::HoloMainWindow(QWidget *parent)
    : QMainWindow(parent), 
    _drivewindows(Mortis::Player::SDL::DriveWindow::Instance())
    , ui(new Ui::HoloMainWindow)
{
    ui->setupUi(this);
    ui->openGLWidget->setUpdatesEnabled(false);
    ui->openGLWidget->setAttribute(Qt::WA_OpaquePaintEvent);

    ui->dockTitleWidget->setTitleBarWidget(new QWidget(ui->dockTitleWidget));
    ui->dockTitleWidget->setWidget(new HoloTitleWidget(ui->dockTitleWidget));

    timer_id = startTimer(250);
    
    connect(ui->openFile, &QAction::triggered, this, &HoloMainWindow::StartOpenFile);
    connect(ui->netMode, &QAction::triggered, this, &HoloMainWindow::StartNetMode);
    
}

HoloMainWindow::~HoloMainWindow()
{
    delete ui;
}

void HoloMainWindow::on_stop_play_clicked()
{
    this->_drivewindows.togglePause();
}

void HoloMainWindow::timerEvent([[maybe_unused]] QTimerEvent * event)
{

    auto& audio_ptr = this->_drivewindows.play_tool.avframe_work[AVMEDIA_TYPE_AUDIO].first;
    if(audio_ptr==nullptr)return;
    if(ui->time_slider->isSliderDown() == false){
        int sec=audio_ptr->pts * this->_drivewindows.play_tool._frameCtxArr[AVMEDIA_TYPE_AUDIO].getSecBaseTime();
        ui->timestamp->setText(QString::asprintf("%02d:%02d", sec / 60, sec % 60));
        ui->time_slider->setValue(sec);
    }
    if (this->_drivewindows.is_pause == false) {
        ui->stop_play->setStyleSheet(s_run_style);
    }
    else {
        ui->stop_play->setStyleSheet(s_stop_style);
    }

}

void HoloMainWindow::resizeEvent([[maybe_unused]] QResizeEvent* event)
{
    this->_drivewindows.ReSize(this->ui->openGLWidget->width(), this->ui->openGLWidget->height());
}


void HoloMainWindow::on_time_slider_sliderReleased()
{
    this->_drivewindows.play_tool.seek_time(ui->time_slider->value());
}

void HoloMainWindow::on_time_slider_sliderMoved(int position)
{
    ui->timestamp->setText(QString::asprintf("%02d:%02d", position/60,position%60));
}

void HoloMainWindow::StartOpenFile()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("打开文件"), "./", tr("video files(*.mp4 *.mkv *.flv);;All files(*.*)"));
    if (filepath.isEmpty() || filepath == "") 
        return;
   
    //if (_drivewindows.play_tool.open(Mo::UTF16ToUTF8((const wchar_t* )filepath.utf16())) == false) {
    //    return;
    //}
	if (_drivewindows.play_tool.open(filepath.toStdString()) == false) {
		return;
	}

    _drivewindows.InitPlayer(ui->openGLWidget->width(), ui->openGLWidget->height(),reinterpret_cast<void*>(ui->openGLWidget->winId()));
    _drivewindows.StartPlayer();
    auto sec = _drivewindows.play_tool.avFmtStream()->duration / AV_TIME_BASE;
    ui->total_time->setText(QString::number(sec / 60) + ":" + QString::number(sec % 60));
    ui->time_slider->setSliderPosition(0);
    ui->time_slider->setMaximum(sec);
}

void HoloMainWindow::StartNetMode()
{
    auto spider_dlg = new NetToFileDialog(this);
    spider_dlg->setAttribute(Qt::WA_DeleteOnClose);
    spider_dlg->exec();
}
void HoloMainWindow::closeWindow()
{
    spdlog::info("closeWindow");
    close();
}
void HoloMainWindow::on_volume_slider_valueChanged(int value)
{
    _drivewindows.volume = value;
}

