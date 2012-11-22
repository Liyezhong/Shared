#ifndef EXTERNALPROCESSCONTROL_CPUBANDWITHEATER_H
#define EXTERNALPROCESSCONTROL_CPUBANDWITHEATER_H

#include <QObject>
#include <QThread>
#include <QTimer>

/// CPU bandwidth consuming class
class Calculator : public QObject {
    Q_OBJECT

public:
    Calculator();
    ~Calculator();

public slots:
    void Calculate();
    void Stop();

private:
    bool m_Calculate; ///< activity flag. true: continue claculation. false: exit working loop.
};

/// This class controls the overall process
class CpuBandwidthEater : public QObject
{
    Q_OBJECT

public:
    CpuBandwidthEater(int runtime, int sleeptime);
    ~CpuBandwidthEater();

public slots:
    void Start();

private slots:
    void OnSleepTimeout();
    void OnRunTimeout();

private:
    int  m_myRunTime;            ///< time to run/calculate
    int  m_mySleepTime;          ///< time to sleep idly
    Calculator *m_myCalculator;  ///< CPU power consuming class
    QThread *m_Thread;           ///< thread to run calculator in
};


#endif // EXTERNALPROCESSCONTROL_CPUBANDWITHEATER_H
