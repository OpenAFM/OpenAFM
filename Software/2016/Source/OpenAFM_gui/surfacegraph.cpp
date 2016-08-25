#include "surfacegraph.h"
#include "def_commands.h"
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>
#include <QFileDialog>
#include <QMessageBox>

#include <QDate>
#include <QTime>


using namespace QtDataVisualization;


SurfaceGraph::SurfaceGraph(Q3DSurface *surface, QWidget *parent,QList<int> parameters)
    :QObject(parent)
{
    this->parameters=parameters;
    m_graph=surface;
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);


    AFM_Proxy=new QSurfaceDataProxy();
    AFM_Series = new QSurface3DSeries(AFM_Proxy);
    AFM_Series->setMeshSmooth(true);
    AFM_Proxy->resetArray(NULL);

    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

}





void SurfaceGraph::fillAFMProxy(QList <QByteArray> data, bool load, QTextStream *stream)
{

    if(load){

        int temp;
        QVector3D vect;
        *stream>>temp;
        sampleCountX=temp;
        bool addingData=true;

        while(!stream->atEnd()&&addingData){

            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);

            for(int i=0;i<sampleCountX;++i){
                *stream>>temp;


                vect.setX(temp);
                *stream>>temp;

                vect.setY(temp);
                *stream>>temp;
                if(temp==0){
                    addingData=false;
                    break;
                }

                vect.setZ(temp);
                (*newRow)[i].setPosition(vect);

            }
            if(addingData){AFM_Proxy->insertRow(AFM_Proxy->rowCount(),newRow);}
        }
        m_graph->axisZ()->setRange(0,AFM_Proxy->rowCount()+2);

    }

    if(!load){

        float stepX = parameters[1];
        qDebug()<<data;
        if(!size_set){
            int line_size=data.size()/2;
            sampleCountX=line_size;
            setAxisScaling(line_size);
            size_set=true;
        }
        if(sampleCountX==data.size()/2){
            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
            int index = 0;
            for (int j = 0; j<sampleCountX; j++) {
                float x = (j * stepX);
                float y = (data.front().toInt() + data.back().toInt())/2;
                data.pop_back();
                data.pop_front();
                float z = AFM_Proxy->rowCount()+1;
                QVector3D vect(x,y,z);
                list.append(vect);
                (*newRow)[index].setPosition(vect);
                index++;

            }
            m_graph->axisZ()->setRange(0,AFM_Proxy->rowCount()+2);

            AFM_Proxy->insertRow(AFM_Proxy->rowCount(),newRow);
            qDebug()<<QTime::currentTime();
            emit sendReady();
        }
        else emit scanFinished();
    }
}



void SurfaceGraph::enableAFMModel()
{

        AFM_Series->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
        AFM_Series->setFlatShadingEnabled(true);

        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");
        m_graph->axisX()->setAutoAdjustRange(true);
        m_graph->axisY()->setAutoAdjustRange(true);
        m_graph->axisZ()->setAutoAdjustRange(true);
        m_graph->axisX()->setLabelAutoRotation(30);
        m_graph->axisY()->setLabelAutoRotation(90);
        m_graph->axisZ()->setLabelAutoRotation(30);

        m_graph->addSeries(AFM_Series);
}

void SurfaceGraph::dataHandler(QByteArray data){

        //data.replace(";","");
        QList <QByteArray> splitData=data.split(',');
        fillAFMProxy(splitData);
        emit fillBitmap(splitData);
}

void SurfaceGraph::saveData(){


    QString filepath = QFileDialog::getExistingDirectory();
    if (!filepath.isNull()){
    QFile file(filepath+"/AFM_Scan_data_"+ QDate::currentDate().toString()+"_"+QTime::currentTime().toString());
    if ( file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &file );
        stream<<sampleCountX<<endl;

        for(QVector3D i:list){
            stream<<i.x()<<" "<<i.y()<<" "<<i.z()<<endl;
        }
        file.close();

    }
    else {QMessageBox::critical(nullptr, tr("Error"), "File Can't Be Opened");}
    }
}


void SurfaceGraph::adjustCameraZ(int angle)
{

    this->m_graph->scene()->activeCamera()->setXRotation(angle/2);
}

void SurfaceGraph::adjustCameraY(int angle)
{
    this->m_graph->scene()->activeCamera()->setYRotation(angle/2);
}

void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void SurfaceGraph::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void SurfaceGraph::setAxisScaling(int sizeX){

    m_graph->axisX()->setRange(0,sizeX);

}

SurfaceGraph::~SurfaceGraph()
{
}
