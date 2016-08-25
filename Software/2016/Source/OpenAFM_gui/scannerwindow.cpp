#include "scannerwindow.h"
#include "tx_rx_protocol.h"
#include "surfacegraph.h"
#include "qcustomplot.h"





scannerwindow::scannerwindow(QList<int> parameters, QMainWindow *parent, bool load, QTextStream* stream):
    QObject(parent)
{
    this->parameters=parameters;
    widget = new QWidget();
    widget->setAttribute(Qt::WA_DeleteOnClose);
    graph = new Q3DSurface();
    container = QWidget::createWindowContainer(graph, widget);
    container->setAttribute(Qt::WA_AcceptTouchEvents);

    widget->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignTop,
            widget->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    bitmapForward=new QCustomPlot(widget);
    bitmapBackward=new QCustomPlot(widget);
    bitmapCombined=new QCustomPlot(widget);

    bitmapForward->setContextMenuPolicy(Qt::CustomContextMenu);
    bitmapBackward->setContextMenuPolicy(Qt::CustomContextMenu);
    bitmapCombined->setContextMenuPolicy(Qt::CustomContextMenu);



    bitmapBackward->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bitmapForward->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bitmapCombined->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    initializeBitmapForward(bitmapForward);
    initializeBitmapBackward(bitmapBackward);
    initializeBitmapCombined(bitmapCombined);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
    }

    QSize screenSize = graph->screen()->size();
    container->setMinimumSize(QSize(screenSize.width() / 3, screenSize.height()/4));
    container->setMaximumSize(screenSize);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setFocusPolicy(Qt::StrongFocus);

    bitmapBackward->setMinimumSize(screenSize.width()/4, screenSize.height()/4);
    bitmapForward->setMinimumSize(screenSize.width()/4, screenSize.height()/4);
    bitmapCombined->setMinimumSize(screenSize.width()/4, screenSize.height()/4);


    QHBoxLayout *hLayout = new QHBoxLayout(widget);
    QVBoxLayout *vLayout = new QVBoxLayout();
    QSplitter *hSplitterMain = new QSplitter();
    QSplitter *vSplitterRight = new QSplitter();

    hLayout->setSizeConstraint(QBoxLayout::SizeConstraint::SetMinimumSize);

    vSplitterRight->addWidget(bitmapForward);
    vSplitterRight->addWidget(bitmapBackward);
    vSplitterRight->addWidget(bitmapCombined);

    vSplitterRight->setOrientation(Qt::Orientation::Vertical);
    vSplitterRight->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    hSplitterMain->setOrientation(Qt::Orientation::Horizontal);

    hSplitterMain->addWidget(container);
    hSplitterMain->addWidget(vSplitterRight);
    hSplitterMain->setStyleSheet("QSplitter::handle {background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,stop:0 rgba(255, 255, 255, 0),stop:0.407273 rgba(200, 200, 200, 255),stop:0.4825 rgba(101, 104, 113, 235), stop:0.6 rgba(255, 255, 255, 0));}");
    vSplitterRight->setStyleSheet("QSplitter::handle {background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0,stop:0 rgba(255, 255, 255, 0),stop:0.407273 rgba(200, 200, 200, 255),stop:0.4825 rgba(101, 104, 113, 235), stop:0.6 rgba(255, 255, 255, 0));}");

    hLayout->addWidget(hSplitterMain);

    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);
    widget->setWindowTitle(QStringLiteral("AFM Scan"));
    QGroupBox *modelGroupBox = new QGroupBox(QStringLiteral("Scan"));



if(!load){
    AFM_Scan_3D_RB = new QPushButton(widget);
    AFM_Scan_3D_RB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    AFM_Scan_3D_RB->setText(QStringLiteral("Start Scan"));
    AFM_Scan_3D_RB->setCheckable(true);
    AFM_Scan_3D_RB->setChecked(false);
}

    BitmapView = new QPushButton(widget);
    BitmapView->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    BitmapView->setText(QStringLiteral("Save BMP"));

    SaveSurface = new QPushButton(widget);
    SaveSurface->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    SaveSurface->setText(QStringLiteral("Save Data"));


    QVBoxLayout *modelVBox = new QVBoxLayout;

   if(!load){ modelVBox->addWidget(AFM_Scan_3D_RB);}
    modelVBox->addWidget(BitmapView);
    modelVBox->addWidget(SaveSurface);
    modelGroupBox->setLayout(modelVBox);

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Selection Mode"));
    selectionGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));


    QRadioButton *modeNoneRB = new QRadioButton(widget);
    modeNoneRB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    modeNoneRB->setText(QStringLiteral("No selection"));
    modeNoneRB->setChecked(false);

    QRadioButton *modeItemRB = new QRadioButton(widget);
    modeItemRB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    modeItemRB->setText(QStringLiteral("Item"));
    modeItemRB->setChecked(false);

    QRadioButton *modeSliceRowRB = new QRadioButton(widget);
    modeSliceRowRB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    modeSliceRowRB->setText(QStringLiteral("Row Slice"));
    modeSliceRowRB->setChecked(false);

    QRadioButton *modeSliceColumnRB = new QRadioButton(widget);
    modeSliceColumnRB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    modeSliceColumnRB->setText(QStringLiteral("Column Slice"));
    modeSliceColumnRB->setChecked(false);

    QVBoxLayout *selectionVBox = new QVBoxLayout;
    selectionVBox->addWidget(modeNoneRB);
    selectionVBox->addWidget(modeItemRB);
    selectionVBox->addWidget(modeSliceRowRB);
    selectionVBox->addWidget(modeSliceColumnRB);
    selectionGroupBox->setLayout(selectionVBox);

    QSlider *axisCameraSliderZ = new QSlider(Qt::Horizontal, widget);
    axisCameraSliderZ->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    axisCameraSliderZ->setMinimum(0);
    axisCameraSliderZ->setMaximum(179);
    axisCameraSliderZ->setTickInterval(1);
    axisCameraSliderZ->setEnabled(true);

    QSlider *axisCameraSliderY = new QSlider(Qt::Horizontal, widget);
    axisCameraSliderY->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    axisCameraSliderY->setMinimum(0);
    axisCameraSliderY->setMaximum(180);
    axisCameraSliderY->setTickInterval(0);
    axisCameraSliderY->setEnabled(true);


    QComboBox *themeList = new QComboBox(widget);
    themeList->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));

    QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Custom gradient"));

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(1.0, Qt::black);
    grBtoY.setColorAt(0.67, Qt::blue);
    grBtoY.setColorAt(0.33, Qt::red);
    grBtoY.setColorAt(0.0, Qt::yellow);
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);

    QPushButton *gradientBtoYPB = new QPushButton(widget);
    gradientBtoYPB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.0, Qt::darkGreen);
    grGtoR.setColorAt(0.5, Qt::yellow);
    grGtoR.setColorAt(0.2, Qt::red);
    grGtoR.setColorAt(0.0, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);

    QPushButton *gradientGtoRPB = new QPushButton(widget);
    gradientGtoRPB->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));

    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorGroupBox->setLayout(colorHBox);

    vLayout->addWidget(modelGroupBox);
    vLayout->addWidget(selectionGroupBox);

    vLayout->addWidget(new QLabel(QStringLiteral("Camera Position")));
    vLayout->addWidget(axisCameraSliderZ);
    vLayout->addWidget(axisCameraSliderY);
    vLayout->addWidget(new QLabel(QStringLiteral("Theme")));
    vLayout->addWidget(themeList);
    vLayout->addWidget(colorGroupBox);

    widget->show();

    modifier= new SurfaceGraph(graph, widget,parameters);


if(!load){

    QObject::connect(parent, SIGNAL(plotDataReceived(QByteArray)),
                     modifier, SLOT(dataHandler(QByteArray)));

    QObject::connect(AFM_Scan_3D_RB, SIGNAL (toggled(bool)),
                     this, SLOT(AFMButtonHandler(bool)));

    QObject::connect(this, SIGNAL (AFMStart()),
                     modifier, SLOT (enableAFMModel()));

    QObject::connect(this, SIGNAL (AFMDone()),
                     parent, SLOT (sendDone()));

    QObject::connect(this, SIGNAL (AFMStart()),
                     parent, SLOT (sendGo()));

    QObject::connect(this, SIGNAL (AFMStart()),
                     parent, SLOT(sendReady()));}

    QObject::connect(widget,SIGNAL(destroyed()),
                     this, SLOT(close()));

    QObject::connect(modifier, SIGNAL (scanFinished()),
                     this, SLOT(endScan()));

    QObject::connect(widget,SIGNAL(destroyed()),
                     parent, SLOT(sendDone()));

    QObject::connect(SaveSurface, SIGNAL (clicked()),
                     modifier, SLOT(saveData()));

    QObject::connect(BitmapView, SIGNAL (clicked()),
                     this, SLOT(getBitmapScreen()));

    QObject::connect(modeNoneRB, &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleModeNone);
    QObject::connect(modeItemRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleModeItem);
    QObject::connect(modeSliceRowRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleModeSliceRow);
    QObject::connect(modeSliceColumnRB,  &QRadioButton::toggled,
                     modifier, &SurfaceGraph::toggleModeSliceColumn);


    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
                     modifier, SLOT(changeTheme(int)));

    QObject::connect(gradientBtoYPB, &QPushButton::pressed,
                     modifier, &SurfaceGraph::setBlackToYellowGradient);
    QObject::connect(gradientGtoRPB, &QPushButton::pressed,
                     modifier, &SurfaceGraph::setGreenToRedGradient);

    QObject::connect(axisCameraSliderZ, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustCameraZ);
    QObject::connect(axisCameraSliderY, &QSlider::valueChanged,
                     modifier, &SurfaceGraph::adjustCameraY);

    QObject::connect(modifier, SIGNAL (fillBitmap(QList <QByteArray>)),
                     this, SLOT (realtimeDataSlotUpdate(QList<QByteArray>)));

    QObject::connect(modifier, SIGNAL (sendReady()),
                     parent, SLOT (sendReady()));


    QObject::connect(bitmapForward, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowForwardContextMenu(const QPoint &)));
    QObject::connect(bitmapBackward, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowBackwardContextMenu(const QPoint &)));
    QObject::connect(bitmapCombined, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ShowCombinedContextMenu(const QPoint &)));

    if(!load){AFM_Scan_3D_RB->setChecked(false);}

    modeItemRB->setChecked(true);
    BitmapView->setEnabled(false);

    themeList->setCurrentIndex(2);
    if(load){
     modifier->enableAFMModel();
     modifier->fillAFMProxy(mock,load,stream);
    }
}




scannerwindow::~scannerwindow()
{
}

void scannerwindow::close(){
    delete this;
}

void scannerwindow::getBitmapScreen(){
    intensitymap* BitmapScreen= new intensitymap(modifier->AFM_Proxy, parameters,widget);
    BitmapScreen->setModal(false);
    BitmapScreen->exec();
}

void scannerwindow::endScan(){
    AFM_Scan_3D_RB->setEnabled(false);
    AFM_Scan_3D_RB->setText("Done");
    BitmapView->setEnabled(true);

}

void scannerwindow::AFMButtonHandler(bool checked){
    if(!checked){
        emit AFMDone();
        AFM_Scan_3D_RB->setText("Stopped");
        AFM_Scan_3D_RB->setStyleSheet("color: #000000;");

        BitmapView->setEnabled(true);

    }
    else{
        emit AFMStart();
        AFM_Scan_3D_RB->setText("End Scan");
        AFM_Scan_3D_RB->setStyleSheet("color: #ffffff;");
        BitmapView->setEnabled(false);

    }
}

void scannerwindow::initializeBitmapForward(QCustomPlot* customPlot){

    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    customPlot->axisRect()->setupFullAxesBox(true);


    colorMapForward = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(colorMapForward);


    colorScaleForward = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScaleForward);
    colorScaleForward->setType(QCPAxis::atRight);
    colorMapForward->setColorScale(colorScaleForward);
    colorScaleForward->axis()->setLabel("Forward Scan");
    colorMapForward->setGradient(QCPColorGradient::gpPolar);

    marginGroupForward = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupForward);
    colorScaleForward->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupForward);
    colorMapForward->setInterpolate(true);
    colorMapForward->data()->setSize(parameters[0],parameters[0]);



}

void scannerwindow::initializeBitmapBackward(QCustomPlot* customPlot){

    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    customPlot->axisRect()->setupFullAxesBox(true);


    colorMapBackward = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(colorMapBackward);


    colorScaleBackward = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScaleBackward);
    colorScaleBackward->setType(QCPAxis::atRight);
    colorMapBackward->setColorScale(colorScaleBackward);
    colorScaleBackward->axis()->setLabel("Backward Scan");

    colorMapBackward->setGradient(QCPColorGradient::gpPolar);

    marginGroupBackward = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupBackward);
    colorScaleBackward->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupBackward);
    colorMapBackward->data()->setSize(parameters[0],parameters[0]);



}

void scannerwindow::initializeBitmapCombined(QCustomPlot* customPlot){

    customPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    customPlot->axisRect()->setupFullAxesBox(true);


    colorMapCombined = new QCPColorMap(customPlot->xAxis, customPlot->yAxis);
    customPlot->addPlottable(colorMapCombined);


    colorScaleCombined = new QCPColorScale(customPlot);
    customPlot->plotLayout()->addElement(0, 1, colorScaleCombined);
    colorScaleCombined->setType(QCPAxis::atRight);
    colorMapCombined->setColorScale(colorScaleCombined);
    colorScaleCombined->axis()->setLabel("Combined Scan");
    colorScaleCombined->axis()->setLabelPadding(3);


    colorMapCombined->setGradient(QCPColorGradient::gpPolar);

    marginGroupCombined = new QCPMarginGroup(customPlot);
    customPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupCombined);
    colorScaleCombined->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupCombined);
    colorMapCombined->data()->setSize(parameters[0],parameters[0]);



}

void scannerwindow::realtimeDataSlotUpdate(QList <QByteArray> data){
    if(data.size()>10){
    realtimeDataSlotForward(data);
    realtimeDataSlotBackward(data);
    realtimeDataSlotCombined(data);
    ++yIndex;
    }
}

void scannerwindow::realtimeDataSlotForward(QList <QByteArray> data)
{
    double z;
    for (int xIndex=0; xIndex<data.size()/2; ++xIndex)
    {
        z = data[xIndex].toDouble();

        colorMapForward->data()->setCell(xIndex, yIndex, z);
    }

    colorMapForward->rescaleDataRange(true);
    bitmapForward->rescaleAxes();
    bitmapForward->xAxis->setScaleRatio(bitmapForward->yAxis,1);
    bitmapForward->replot();
 }

void scannerwindow::realtimeDataSlotBackward(QList <QByteArray> data)
{
    double z;
    for (int xIndex=0; xIndex<data.size()/2; ++xIndex)
    {
        z = data[xIndex+data.size()/2].toDouble();

        colorMapBackward->data()->setCell(xIndex, yIndex, z);
    }

    colorMapBackward->rescaleDataRange(true);
    bitmapBackward->rescaleAxes();
    bitmapBackward->xAxis->setScaleRatio(bitmapBackward->yAxis,1);
    bitmapBackward->replot();
 }

void scannerwindow::realtimeDataSlotCombined(QList <QByteArray> data)
{
    double z;
    int size=data.size()/2;
    for (int xIndex=0; xIndex<size; ++xIndex)
    {
        float z = (data.front().toInt() + data.back().toInt())/2;
        data.pop_back();
        data.pop_front();
        colorMapCombined->data()->setCell(xIndex, yIndex, z);
    }

    colorMapCombined->rescaleDataRange(true);
    bitmapCombined->rescaleAxes();
    bitmapCombined->xAxis->setScaleRatio(bitmapCombined->yAxis,1);
    bitmapCombined->replot();
 }

void scannerwindow::ShowForwardContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"), bitmapForward);

   QAction invert("Invert");
   QSignalMapper invertMapper;

   connect(&invertMapper, SIGNAL(mapped(int)),this,SLOT(invertPlot(int)));
   connect(&invert, SIGNAL(triggered()), &invertMapper, SLOT(map()));

   invertMapper.setMapping(&invert,0);
   contextMenu.addAction(&invert);

   QAction interpolate("Switch Interpolation");
   QSignalMapper interpolateMapper;

   connect(&interpolateMapper, SIGNAL(mapped(int)),this,SLOT(interpolatePlot(int)));
   connect(&interpolate, SIGNAL(triggered()), &interpolateMapper, SLOT(map()));
   interpolateMapper.setMapping(&interpolate,0);
   contextMenu.addAction(&interpolate);

   QMenu colorSubMenu(tr("Select Color Map"));
   contextMenu.addMenu(&colorSubMenu);

   QAction polar("Polar");
   QSignalMapper polarMapper;

   connect(&polarMapper, SIGNAL(mapped(int)),this,SLOT(polarPlot(int)));
   connect(&polar, SIGNAL(triggered()), &polarMapper, SLOT(map()));
   polarMapper.setMapping(&polar,0);
   colorSubMenu.addAction(&polar);

   QAction thermal("Thermal");
   QSignalMapper thermalMapper;
   connect(&thermalMapper, SIGNAL(mapped(int)),this,SLOT(thermalPlot(int)));
   connect(&thermal, SIGNAL(triggered()), &thermalMapper, SLOT(map()));
   thermalMapper.setMapping(&thermal,0);
   colorSubMenu.addAction(&thermal);

   QAction hot("Hot");
   QSignalMapper hotMapper;
   connect(&hotMapper, SIGNAL(mapped(int)),this,SLOT(hotPlot(int)));
   connect(&hot, SIGNAL(triggered()), &hotMapper, SLOT(map()));
   hotMapper.setMapping(&hot,0);
   colorSubMenu.addAction(&hot);

   QAction cold("Cold");
   QSignalMapper coldMapper;
   connect(&coldMapper, SIGNAL(mapped(int)),this,SLOT(coldPlot(int)));
   connect(&cold, SIGNAL(triggered()), &coldMapper, SLOT(map()));
   coldMapper.setMapping(&cold,0);
   colorSubMenu.addAction(&cold);

   QAction grayscale("Grayscale");
   QSignalMapper grayscaleMapper;
   connect(&grayscaleMapper, SIGNAL(mapped(int)),this,SLOT(grayscalePlot(int)));
   connect(&grayscale, SIGNAL(triggered()), &grayscaleMapper, SLOT(map()));
   grayscaleMapper.setMapping(&grayscale,0);
   colorSubMenu.addAction(&grayscale);
   colorSubMenu.addAction(&grayscale);


   contextMenu.exec(bitmapForward->mapToGlobal(pos));
}

void scannerwindow::ShowBackwardContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"),bitmapBackward);

   QAction invert("Invert");
   QSignalMapper invertMapper;

   connect(&invertMapper, SIGNAL(mapped(int)),this,SLOT(invertPlot(int)));
   connect(&invert, SIGNAL(triggered()), &invertMapper, SLOT(map()));

   invertMapper.setMapping(&invert,1);
   contextMenu.addAction(&invert);

   QAction interpolate("Switch Interpolation");
   QSignalMapper interpolateMapper;

   connect(&interpolateMapper, SIGNAL(mapped(int)),this,SLOT(interpolatePlot(int)));
   connect(&interpolate, SIGNAL(triggered()), &interpolateMapper, SLOT(map()));
   interpolateMapper.setMapping(&interpolate,1);
   contextMenu.addAction(&interpolate);

   QMenu colorSubMenu(tr("Select Color Map"));
   contextMenu.addMenu(&colorSubMenu);

   QAction polar("Polar");
   QSignalMapper polarMapper;

   connect(&polarMapper, SIGNAL(mapped(int)),this,SLOT(polarPlot(int)));
   connect(&polar, SIGNAL(triggered()), &polarMapper, SLOT(map()));
   polarMapper.setMapping(&polar,1);
   colorSubMenu.addAction(&polar);

   QAction thermal("Thermal");
   QSignalMapper thermalMapper;
   connect(&thermalMapper, SIGNAL(mapped(int)),this,SLOT(thermalPlot(int)));
   connect(&thermal, SIGNAL(triggered()), &thermalMapper, SLOT(map()));
   thermalMapper.setMapping(&thermal,1);
   colorSubMenu.addAction(&thermal);

   QAction hot("Hot");
   QSignalMapper hotMapper;
   connect(&hotMapper, SIGNAL(mapped(int)),this,SLOT(hotPlot(int)));
   connect(&hot, SIGNAL(triggered()), &hotMapper, SLOT(map()));
   hotMapper.setMapping(&hot,1);
   colorSubMenu.addAction(&hot);

   QAction cold("Cold");
   QSignalMapper coldMapper;
   connect(&coldMapper, SIGNAL(mapped(int)),this,SLOT(coldPlot(int)));
   connect(&cold, SIGNAL(triggered()), &coldMapper, SLOT(map()));
   coldMapper.setMapping(&cold,1);
   colorSubMenu.addAction(&cold);

   QAction grayscale("Grayscale");
   QSignalMapper grayscaleMapper;
   connect(&grayscaleMapper, SIGNAL(mapped(int)),this,SLOT(grayscalePlot(int)));
   connect(&grayscale, SIGNAL(triggered()), &grayscaleMapper, SLOT(map()));
   grayscaleMapper.setMapping(&grayscale,1);
   colorSubMenu.addAction(&grayscale);
   colorSubMenu.addAction(&grayscale);


   contextMenu.exec(bitmapBackward->mapToGlobal(pos));
}

void scannerwindow::ShowCombinedContextMenu(const QPoint &pos)
{
   QMenu contextMenu(tr("Context menu"));

   QAction invert("Invert");
   QSignalMapper invertMapper;

   connect(&invertMapper, SIGNAL(mapped(int)),this,SLOT(invertPlot(int)));
   connect(&invert, SIGNAL(triggered()), &invertMapper, SLOT(map()));

   invertMapper.setMapping(&invert,2);
   contextMenu.addAction(&invert);

   QAction interpolate("Switch Interpolation");
   QSignalMapper interpolateMapper;

   connect(&interpolateMapper, SIGNAL(mapped(int)),this,SLOT(interpolatePlot(int)));
   connect(&interpolate, SIGNAL(triggered()), &interpolateMapper, SLOT(map()));
   interpolateMapper.setMapping(&interpolate,2);
   contextMenu.addAction(&interpolate);

   QMenu colorSubMenu(tr("Select Color Map"));
   contextMenu.addMenu(&colorSubMenu);

   QAction polar("Polar");
   QSignalMapper polarMapper;

   connect(&polarMapper, SIGNAL(mapped(int)),this,SLOT(polarPlot(int)));
   connect(&polar, SIGNAL(triggered()), &polarMapper, SLOT(map()));
   polarMapper.setMapping(&polar,2);
   colorSubMenu.addAction(&polar);

   QAction thermal("Thermal");
   QSignalMapper thermalMapper;
   connect(&thermalMapper, SIGNAL(mapped(int)),this,SLOT(thermalPlot(int)));
   connect(&thermal, SIGNAL(triggered()), &thermalMapper, SLOT(map()));
   thermalMapper.setMapping(&thermal,2);
   colorSubMenu.addAction(&thermal);

   QAction hot("Hot");
   QSignalMapper hotMapper;
   connect(&hotMapper, SIGNAL(mapped(int)),this,SLOT(hotPlot(int)));
   connect(&hot, SIGNAL(triggered()), &hotMapper, SLOT(map()));
   hotMapper.setMapping(&hot,2);
   colorSubMenu.addAction(&hot);

   QAction cold("Cold");
   QSignalMapper coldMapper;
   connect(&coldMapper, SIGNAL(mapped(int)),this,SLOT(coldPlot(int)));
   connect(&cold, SIGNAL(triggered()), &coldMapper, SLOT(map()));
   coldMapper.setMapping(&cold,2);
   colorSubMenu.addAction(&cold);

   QAction grayscale("Grayscale");
   QSignalMapper grayscaleMapper;
   connect(&grayscaleMapper, SIGNAL(mapped(int)),this,SLOT(grayscalePlot(int)));
   connect(&grayscale, SIGNAL(triggered()), &grayscaleMapper, SLOT(map()));
   grayscaleMapper.setMapping(&grayscale,2);
   colorSubMenu.addAction(&grayscale);
   colorSubMenu.addAction(&grayscale);


   contextMenu.exec(bitmapCombined->mapToGlobal(pos));
}

void scannerwindow::invertPlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(colorMapForward->gradient().inverted());
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(colorMapBackward->gradient().inverted());
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(colorMapCombined->gradient().inverted());
        bitmapCombined->replot();
        break;
    }


}

void scannerwindow::interpolatePlot(int index){
    switch(index){
    case 0:
        colorMapForward->setInterpolate(!colorMapForward->interpolate());
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setInterpolate(!colorMapBackward->interpolate());
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setInterpolate(!colorMapCombined->interpolate());
        bitmapCombined->replot();
        break;
    }


}

void scannerwindow::polarPlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(QCPColorGradient::gpPolar);
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(QCPColorGradient::gpPolar);
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(QCPColorGradient::gpPolar);
        bitmapCombined->replot();
        break;
    }

}

void scannerwindow::hotPlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(QCPColorGradient::gpHot);
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(QCPColorGradient::gpHot);
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(QCPColorGradient::gpHot);
        bitmapCombined->replot();
        break;
    }

}

void scannerwindow::coldPlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(QCPColorGradient::gpCold);
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(QCPColorGradient::gpCold);
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(QCPColorGradient::gpCold);
        bitmapCombined->replot();
        break;
    }

}

void scannerwindow::thermalPlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(QCPColorGradient::gpThermal);
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(QCPColorGradient::gpThermal);
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(QCPColorGradient::gpThermal);
        bitmapCombined->replot();
        break;
    }

}

void scannerwindow::grayscalePlot(int index){
    switch(index){
    case 0:
        colorMapForward->setGradient(QCPColorGradient::gpGrayscale);
        bitmapForward->replot();
        break;
    case 1:
        colorMapBackward->setGradient(QCPColorGradient::gpGrayscale);
        bitmapBackward->replot();
        break;
    case 2:
        colorMapCombined->setGradient(QCPColorGradient::gpGrayscale);
        bitmapCombined->replot();
        break;
    }

}



void scannerwindow::saveImage(){

   }


