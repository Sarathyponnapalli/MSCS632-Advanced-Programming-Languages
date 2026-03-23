#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include "RideSystem.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onBookRide();
    void onDistChanged();
    void onPaxChanged();
    void switchPage(int idx);

private:
    RideSystem sys;
    QString    currentType = "standard";

    QStackedWidget*     pageStack  = nullptr;
    QLabel*             pageTitle  = nullptr;
    QList<QPushButton*> navBtns;

    QPushButton* navDashboard = nullptr;
    QPushButton* navBook      = nullptr;
    QPushButton* navTrips     = nullptr;
    QPushButton* navDrivers   = nullptr;
    QPushButton* navRiders    = nullptr;

    QLabel*      lblTotalRides = nullptr;
    QLabel*      lblRevenue    = nullptr;
    QLabel*      lblAvgFare    = nullptr;
    QLabel*      lblMiles      = nullptr;
    QListWidget* dashTripsList = nullptr;
    QTextEdit*   activityLog   = nullptr;

    QPushButton* btnStandard  = nullptr;
    QPushButton* btnPremium   = nullptr;
    QPushButton* btnShared    = nullptr;
    QComboBox*   comboRider   = nullptr;
    QComboBox*   comboDriver  = nullptr;
    QLineEdit*   entryPickup  = nullptr;
    QLineEdit*   entryDropoff = nullptr;
    QLineEdit*   entryDist    = nullptr;
    QLineEdit*   entryPax     = nullptr;
    QWidget*     paxWidget    = nullptr;
    QLabel*      lblFareAmount  = nullptr;
    QLabel*      lblFareFormula = nullptr;
    QListWidget* bookTripsList  = nullptr;

    QListWidget* allTripsList = nullptr;
    QListWidget* driversList  = nullptr;
    QListWidget* ridersList   = nullptr;

    void loadAndSetupUi();
    void connectSignals();
    void seedData();
    void rebuildAll();
    void rebuildTrips(QListWidget* lw);
    void rebuildDrivers();
    void rebuildRiders();
    void updateMetrics();
    void updateFarePreview();
    void updateNavState(int activeIdx);
    void setTypeActive(const QString& type);
    void logMsg(const QString& msg);
    QString typeColor(const QString& t);
    QString typeBg(const QString& t);
    QString f2(double v);

    // Find a child widget by name — asserts if missing
    template<class T>
    T* W(const QString& name) {
        T* w = centralWidget()->findChild<T*>(name);
        if (!w) qFatal("Widget '%s' not found in .ui", qPrintable(name));
        return w;
    }
};
