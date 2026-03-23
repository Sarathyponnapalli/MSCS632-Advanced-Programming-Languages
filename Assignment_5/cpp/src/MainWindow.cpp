#include "MainWindow.h"
#include "RideTypes.h"

#include <QUiLoader>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDateTime>
#include <QStyle>

// ─── Stylesheet ───────────────────────────────────────────────────────────────
static const QString CSS = R"CSS(

QMainWindow                { background: #F6F6F6; }
QWidget#centralWidget      { background: #F6F6F6; }
QWidget#mainArea           { background: #F6F6F6; }
QWidget#pageDashboard, QWidget#pageBook,
QWidget#pageTrips, QWidget#pageDrivers,
QWidget#pageRiders         { background: #F6F6F6; }
QStackedWidget             { background: #F6F6F6; }

QWidget#sidebar            { background: #1A1A1A; min-width:220px; max-width:220px; }
QWidget#topbar             { background: #FFFFFF; border-bottom: 1px solid #E2E2E2; }

QWidget#cardTotalRides, QWidget#cardRevenue,
QWidget#cardAvgFare,    QWidget#cardMiles,
QWidget#cardRecentTrips, QWidget#cardActivityLog,
QWidget#bookFormCard,    QWidget#bookSideCard,
QWidget#cardAllTrips,    QWidget#cardDrivers,
QWidget#cardRiders       { background:#FFFFFF; border:1px solid #E2E2E2; border-radius:12px; }

QWidget#fareBox            { background:#F8F8F8; border:1px solid #E2E2E2; border-radius:10px; }
QWidget#paxWidget          { background:transparent; }

QLabel#logoLabel    { font-size:20px; font-weight:700; color:#FFFFFF; background:transparent; }
QLabel#logoSub      { font-size:10px; color:#777777;  background:transparent; }
QLabel#versionLabel { font-size:10px; color:#555555;  background:transparent; }

QLabel#pageTitle    { font-size:18px; font-weight:700; color:#1A1A1A; background:transparent; }
QLabel#statusChip   { font-size:11px; font-weight:700; color:#00A651; background:transparent; }

QLabel#lblTotalRidesLabel, QLabel#lblRevenueLabel,
QLabel#lblAvgFareLabel,    QLabel#lblMilesLabel    { font-size:11px; font-weight:600; color:#767676; background:transparent; }
QLabel#lblTotalRides, QLabel#lblRevenue,
QLabel#lblAvgFare,    QLabel#lblMiles              { font-size:28px; font-weight:700; color:#1A1A1A; background:transparent; }

QLabel#recentTripsTitle, QLabel#activityLogTitle,
QLabel#bookFormTitle,    QLabel#bookSideTitle,
QLabel#allTripsTitle,    QLabel#driversTitle,
QLabel#ridersTitle       { font-size:15px; font-weight:700; color:#1A1A1A; background:transparent; }
QLabel#allTripsSub       { font-size:12px; color:#767676; background:transparent; }

QLabel#rideTypeLabel, QLabel#riderLabel,   QLabel#driverLabel,
QLabel#pickupLabel,   QLabel#dropoffLabel, QLabel#distLabel,
QLabel#paxLabel,      QLabel#farePriceLabel { font-size:11px; font-weight:600; color:#767676; background:transparent; }

QLabel#lblFareAmount  { font-size:28px; font-weight:700; color:#1A1A1A; background:transparent; }
QLabel#lblFareFormula { font-size:12px; color:#767676; background:transparent; }

QFrame#sidebarDivider, QFrame#recentTripsDivider, QFrame#activityLogDivider,
QFrame#bookFormDivider, QFrame#bookSideDivider,    QFrame#allTripsDivider,
QFrame#driversDivider,  QFrame#ridersDivider       { background:#E8E8E8; border:none; max-height:1px; }

QPushButton#navDashboard, QPushButton#navBook, QPushButton#navTrips,
QPushButton#navDrivers,   QPushButton#navRiders {
    background:transparent; color:#888888; border:none; border-radius:8px;
    padding:10px 14px; font-size:13px; font-weight:500; text-align:left;
}
QPushButton#navDashboard:hover, QPushButton#navBook:hover,
QPushButton#navTrips:hover,     QPushButton#navDrivers:hover,
QPushButton#navRiders:hover     { background:#2D2D2D; color:#FFFFFF; }
QPushButton#navDashboard[active="true"], QPushButton#navBook[active="true"],
QPushButton#navTrips[active="true"],     QPushButton#navDrivers[active="true"],
QPushButton#navRiders[active="true"]    { background:#2D2D2D; color:#FFFFFF; }

QPushButton#btnStandard, QPushButton#btnPremium, QPushButton#btnShared {
    background:#F6F6F6; color:#767676; border:1px solid #E2E2E2;
    border-radius:10px; font-size:13px; font-weight:500;
}
QPushButton#btnStandard:hover, QPushButton#btnPremium:hover,
QPushButton#btnShared:hover    { border-color:#1A1A1A; color:#1A1A1A; background:#FFFFFF; }
QPushButton#btnStandard[active="true"] { background:#E6F5ED; color:#00A651; border:2px solid #00A651; font-weight:700; }
QPushButton#btnPremium[active="true"]  { background:#FEF3E2; color:#C9860A; border:2px solid #C9860A; font-weight:700; }
QPushButton#btnShared[active="true"]   { background:#EBF0FE; color:#276EF1; border:2px solid #276EF1; font-weight:700; }

QPushButton#btnBook {
    background:#000000; color:#FFFFFF; border:none;
    border-radius:8px; font-size:14px; font-weight:700;
}
QPushButton#btnBook:hover  { background:#333333; }
QPushButton#btnBook:pressed { background:#555555; }

QLineEdit {
    background:#FFFFFF; color:#1A1A1A; border:1px solid #DDDDDD;
    border-radius:8px; padding:9px 12px; font-size:13px;
    selection-background-color:#000000; selection-color:#FFFFFF;
}
QLineEdit:focus { border:1.5px solid #1A1A1A; }

QComboBox {
    background:#FFFFFF; color:#1A1A1A; border:1px solid #DDDDDD;
    border-radius:8px; padding:9px 12px; font-size:13px; min-height:22px;
}
QComboBox:focus { border:1.5px solid #1A1A1A; }
QComboBox::drop-down { border:none; width:24px; }
QComboBox QAbstractItemView {
    background:#FFFFFF; color:#1A1A1A; border:1px solid #E2E2E2;
    selection-background-color:#F0F0F0; selection-color:#1A1A1A; outline:none;
}

QListWidget                { background:transparent; border:none; outline:none; font-size:13px; color:#1A1A1A; }
QListWidget::item          { padding:0px; border:none; }
QListWidget::item:hover    { background:transparent; }
QListWidget::item:selected { background:transparent; }

QTextEdit#activityLog {
    background:#FAFAFA; color:#333333; border:1px solid #E8E8E8;
    border-radius:8px; padding:10px; font-size:11px;
}

QScrollBar:vertical           { background:transparent; width:4px; margin:0; }
QScrollBar::handle:vertical   { background:#CCCCCC; border-radius:2px; min-height:20px; }
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical { height:0; }

QSplitter::handle             { background:#EEEEEE; }
)CSS";

// ─── Helpers ──────────────────────────────────────────────────────────────────
QString MainWindow::f2(double v) { return QString::asprintf("$%.2f", v); }

QString MainWindow::typeColor(const QString& t) {
    if (t == "Premium" || t == "premium") return "#C9860A";
    if (t == "Shared"  || t == "shared")  return "#276EF1";
    return "#00A651";
}
QString MainWindow::typeBg(const QString& t) {
    if (t == "Premium" || t == "premium") return "#FEF3E2";
    if (t == "Shared"  || t == "shared")  return "#EBF0FE";
    return "#E6F5ED";
}

// ─── Constructor ─────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("RideShare  —  C++ OOP  ·  Qt6");
    resize(1200, 760);
    setMinimumSize(960, 640);

    loadAndSetupUi();   // loads .ui, sets stylesheet, finds all widgets, wires signals
    seedData();
    rebuildAll();
    updateMetrics();
    updateNavState(0);
    logMsg("RideShare ready — 3 drivers, 3 riders, 5 trips loaded.");
}

// ─── loadAndSetupUi ───────────────────────────────────────────────────────────
void MainWindow::loadAndSetupUi() {
    // 1. Find the .ui file
    QStringList candidates = {
        QCoreApplication::applicationDirPath() + "/MainWindow.ui",
        QCoreApplication::applicationDirPath() + "/../ui/MainWindow.ui",
        QDir::currentPath() + "/MainWindow.ui",
        QDir::currentPath() + "/ui/MainWindow.ui",
    };
    QString uiPath;
    for (const QString& p : candidates) {
        if (QFile::exists(p)) { uiPath = QDir::cleanPath(p); break; }
    }
    if (uiPath.isEmpty()) {
        QMessageBox::critical(nullptr, "UI File Missing",
            "MainWindow.ui not found.\n"
            "Make sure it is in the same folder as the executable.");
        std::exit(1);
    }

    // 2. Load it — QUiLoader returns a plain QWidget tree
    QFile file(uiPath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "UI File Error",
            "Cannot open: " + uiPath + "\n" + file.errorString());
        std::exit(1);
    }
    QUiLoader loader;
    QWidget* ui = loader.load(&file, nullptr);  // parent = nullptr, we manage it
    file.close();

    if (!ui) {
        QMessageBox::critical(nullptr, "UI Load Failed", loader.errorString());
        std::exit(1);
    }

    // 3. Apply stylesheet to the loaded widget (not qApp — avoids QDialog contamination)
    ui->setStyleSheet(CSS);

    // 4. Embed it as the central widget and fill the window
    setCentralWidget(ui);

    // 5. Find every named widget — assertion fires immediately if a name is wrong
    pageTitle  = W<QLabel>        ("pageTitle");
    pageStack  = W<QStackedWidget>("pageStack");

    navDashboard = W<QPushButton>("navDashboard");
    navBook      = W<QPushButton>("navBook");
    navTrips     = W<QPushButton>("navTrips");
    navDrivers   = W<QPushButton>("navDrivers");
    navRiders    = W<QPushButton>("navRiders");
    navBtns      = { navDashboard, navBook, navTrips, navDrivers, navRiders };

    lblTotalRides = W<QLabel>      ("lblTotalRides");
    lblRevenue    = W<QLabel>      ("lblRevenue");
    lblAvgFare    = W<QLabel>      ("lblAvgFare");
    lblMiles      = W<QLabel>      ("lblMiles");
    dashTripsList = W<QListWidget> ("dashTripsList");
    activityLog   = W<QTextEdit>   ("activityLog");

    btnStandard   = W<QPushButton> ("btnStandard");
    btnPremium    = W<QPushButton> ("btnPremium");
    btnShared     = W<QPushButton> ("btnShared");
    comboRider    = W<QComboBox>   ("comboRider");
    comboDriver   = W<QComboBox>   ("comboDriver");
    entryPickup   = W<QLineEdit>   ("entryPickup");
    entryDropoff  = W<QLineEdit>   ("entryDropoff");
    entryDist     = W<QLineEdit>   ("entryDist");
    entryPax      = W<QLineEdit>   ("entryPax");
    paxWidget     = W<QWidget>     ("paxWidget");
    lblFareAmount  = W<QLabel>     ("lblFareAmount");
    lblFareFormula = W<QLabel>     ("lblFareFormula");
    bookTripsList  = W<QListWidget>("bookTripsList");

    allTripsList = W<QListWidget>("allTripsList");
    driversList  = W<QListWidget>("driversList");
    ridersList   = W<QListWidget>("ridersList");

    paxWidget->setVisible(false);

    // 6. Wire all signals in C++ (NOT in the .ui — QUiLoader can't resolve our slots)
    connectSignals();
}

// ─── connectSignals ───────────────────────────────────────────────────────────
void MainWindow::connectSignals() {
    // Navigation
    connect(navDashboard, &QPushButton::clicked, this, [this]{ switchPage(0); });
    connect(navBook,      &QPushButton::clicked, this, [this]{ switchPage(1); });
    connect(navTrips,     &QPushButton::clicked, this, [this]{ switchPage(2); });
    connect(navDrivers,   &QPushButton::clicked, this, [this]{ switchPage(3); });
    connect(navRiders,    &QPushButton::clicked, this, [this]{ switchPage(4); });

    // Ride type selection
    connect(btnStandard, &QPushButton::clicked, this, [this]{ setTypeActive("standard"); });
    connect(btnPremium,  &QPushButton::clicked, this, [this]{ setTypeActive("premium");  });
    connect(btnShared,   &QPushButton::clicked, this, [this]{ setTypeActive("shared");   });

    // Live fare preview
    connect(entryDist, &QLineEdit::textChanged, this, &MainWindow::onDistChanged);
    connect(entryPax,  &QLineEdit::textChanged, this, &MainWindow::onPaxChanged);

    // Book button
    connect(W<QPushButton>("btnBook"), &QPushButton::clicked, this, &MainWindow::onBookRide);

    // Set Standard as default active type
    setTypeActive("standard");
}

// ─── setTypeActive ────────────────────────────────────────────────────────────
void MainWindow::setTypeActive(const QString& type) {
    currentType = type;

    auto refresh = [](QPushButton* b, bool active) {
        b->setProperty("active", active);
        b->style()->unpolish(b);
        b->style()->polish(b);
    };
    refresh(btnStandard, type == "standard");
    refresh(btnPremium,  type == "premium");
    refresh(btnShared,   type == "shared");

    paxWidget->setVisible(type == "shared");
    updateFarePreview();
}

// ─── switchPage ───────────────────────────────────────────────────────────────
void MainWindow::switchPage(int idx) {
    pageStack->setCurrentIndex(idx);
    static const QStringList titles = {
        "Dashboard", "Book a Ride", "All Trips", "Drivers", "Riders"
    };
    pageTitle->setText(titles.value(idx, ""));
    updateNavState(idx);
}

void MainWindow::updateNavState(int active) {
    for (int i = 0; i < navBtns.size(); i++) {
        navBtns[i]->setProperty("active", i == active);
        navBtns[i]->style()->unpolish(navBtns[i]);
        navBtns[i]->style()->polish(navBtns[i]);
    }
}

// ─── Seed data ────────────────────────────────────────────────────────────────
void MainWindow::seedData() {
    sys.addDriver(std::make_shared<Driver>(1, "Alice Johnson", 4.9));
    sys.addDriver(std::make_shared<Driver>(2, "Carlos Rivera", 4.7));
    sys.addDriver(std::make_shared<Driver>(3, "Priya Sharma",  4.8));
    sys.addRider (std::make_shared<Rider>(101, "Bob Chen"));
    sys.addRider (std::make_shared<Rider>(102, "Diana Park"));
    sys.addRider (std::make_shared<Rider>(103, "Ethan Nguyen"));
    sys.bookStandardRide(101, 1, "Peoria Station",  "Caterpillar HQ",  4.2);
    sys.bookPremiumRide (102, 2, "Embassy Suites",  "Civic Center",    2.8);
    sys.bookSharedRide  (103, 3, "UIC Campus",      "O'Hare Airport", 11.5, 4);
    sys.bookStandardRide(101, 2, "Oak Park",        "Wrigley Field",   6.0);
    sys.bookPremiumRide (102, 1, "Midway Airport",  "The Loop",        9.3);
}

// ─── Fare preview ─────────────────────────────────────────────────────────────
void MainWindow::onDistChanged() { updateFarePreview(); }
void MainWindow::onPaxChanged()  { updateFarePreview(); }

void MainWindow::updateFarePreview() {
    bool ok;
    double dist = entryDist->text().toDouble(&ok);
    if (!ok || dist <= 0) {
        lblFareAmount->setText("—");
        lblFareAmount->setStyleSheet("font-size:30px;font-weight:700;color:#1A1A1A;");
        lblFareFormula->setText("Enter distance to see fare");
        return;
    }
    int pax = std::max(2, entryPax->text().toInt());
    double fare = 0.0;
    QString formula;

    if (currentType == "standard") {
        fare    = 2.50 + 1.20 * dist;
        formula = QString("$2.50 base  +  $1.20 x %1 mi").arg(dist, 0, 'f', 1);
    } else if (currentType == "premium") {
        fare    = 5.00 + 2.80 * dist + 4.00;
        formula = QString("$5.00  +  $2.80 x %1 mi  +  $4.00 luxury").arg(dist, 0, 'f', 1);
    } else {
        fare    = (2.00 + 1.00 * dist) / pax;
        formula = QString("($2.00 + $1.00 x %1 mi)  /  %2 passengers").arg(dist, 0, 'f', 1).arg(pax);
    }

    QString col = typeColor(currentType);
    lblFareAmount->setText(f2(fare));
    lblFareAmount->setStyleSheet(
        QString("font-size:30px;font-weight:700;color:%1;").arg(col));
    lblFareFormula->setText(formula);
}

// ─── Book ride ────────────────────────────────────────────────────────────────
void MainWindow::onBookRide() {
    QString pu   = entryPickup->text().trimmed();
    QString dr   = entryDropoff->text().trimmed();
    QString dStr = entryDist->text().trimmed();

    if (pu.isEmpty() || dr.isEmpty() || dStr.isEmpty()) {
        QMessageBox::warning(this, "Missing Info",
            "Please fill in pickup, dropoff, and distance.");
        return;
    }
    bool ok;
    double dist = dStr.toDouble(&ok);
    if (!ok || dist <= 0) {
        QMessageBox::warning(this, "Invalid Distance", "Distance must be greater than 0.");
        return;
    }
    int pax = std::max(2, entryPax->text().toInt());

    int riderIDs[]  = {101, 102, 103};
    int driverIDs[] = {1, 2, 3};
    int rID = riderIDs [std::max(0, comboRider->currentIndex())];
    int dID = driverIDs[std::max(0, comboDriver->currentIndex())];

    std::shared_ptr<Ride> ride;
    std::string p = pu.toStdString(), d = dr.toStdString();
    if      (currentType == "standard") ride = sys.bookStandardRide(rID, dID, p, d, dist);
    else if (currentType == "premium")  ride = sys.bookPremiumRide (rID, dID, p, d, dist);
    else                                ride = sys.bookSharedRide  (rID, dID, p, d, dist, pax);

    logMsg(QString("Trip #%1  %2 -> %3  %4")
           .arg(ride->getRideID()).arg(pu).arg(dr).arg(f2(ride->fare())));

    entryPickup->clear();
    entryDropoff->clear();
    entryDist->clear();
    updateFarePreview();
    rebuildAll();
    updateMetrics();

    QMessageBox::information(this, "Trip Confirmed",
        QString("Your %1 ride is confirmed!\n\n%2  ->  %3\nFare: %4")
        .arg(currentType).arg(pu).arg(dr).arg(f2(ride->fare())));
}

// ─── Trip row widget ──────────────────────────────────────────────────────────
static void addTripRow(QListWidget* lw, const Ride& r,
                       const QString& col, const QString& bg) {
    QWidget* row = new QWidget;
    row->setStyleSheet(
        "QWidget { background:#FFFFFF; border:1px solid #E2E2E2; border-radius:10px; }"
        "QWidget:hover { background:#F8F8F8; }");

    QHBoxLayout* hl = new QHBoxLayout(row);
    hl->setContentsMargins(14, 12, 14, 12);
    hl->setSpacing(14);

    // Type badge
    QString type = QString::fromStdString(r.rideType());
    QLabel* badge = new QLabel(type);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedSize(78, 26);
    badge->setStyleSheet(
        QString("background:%1; color:%2; border-radius:13px;"
                "font-size:11px; font-weight:700; border:none;")
        .arg(bg).arg(col));

    // Route + meta
    QVBoxLayout* mid = new QVBoxLayout;
    mid->setSpacing(2);
    QLabel* route = new QLabel(
        QString("<b style='color:#1A1A1A;'>%1</b>"
                "<span style='color:#ABABAB;'> &rarr; </span>"
                "<b style='color:#1A1A1A;'>%2</b>")
        .arg(QString::fromStdString(r.getPickup()))
        .arg(QString::fromStdString(r.getDropoff())));
    route->setTextFormat(Qt::RichText);
    route->setStyleSheet("background:transparent; border:none;");

    QLabel* meta = new QLabel(
        QString("<span style='color:#767676; font-size:11px;'>"
                "#%1 &nbsp;&middot;&nbsp; %2 mi</span>")
        .arg(r.getRideID())
        .arg(r.getDistance(), 0, 'f', 1));
    meta->setTextFormat(Qt::RichText);
    meta->setStyleSheet("background:transparent; border:none;");

    mid->addWidget(route);
    mid->addWidget(meta);

    // Fare
    QLabel* fare = new QLabel(QString::asprintf("$%.2f", r.fare()));
    fare->setStyleSheet(
        QString("font-size:16px; font-weight:700; color:%1;"
                "background:transparent; border:none;").arg(col));
    fare->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    hl->addWidget(badge);
    hl->addLayout(mid, 1);
    hl->addWidget(fare);

    QListWidgetItem* item = new QListWidgetItem(lw);
    item->setSizeHint(QSize(0, 64));
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    lw->setItemWidget(item, row);
}

// ─── Rebuild ──────────────────────────────────────────────────────────────────
void MainWindow::rebuildTrips(QListWidget* lw) {
    if (!lw) return;
    lw->clear();
    const auto& rides = sys.getAllRides();
    if (rides.empty()) {
        auto* item = new QListWidgetItem("No trips yet.");
        item->setForeground(QColor("#ABABAB"));
        lw->addItem(item);
        return;
    }
    for (auto it = rides.rbegin(); it != rides.rend(); ++it) {
        const Ride& r = **it;
        QString t = QString::fromStdString(r.rideType());
        addTripRow(lw, r, typeColor(t), typeBg(t));
    }
}

void MainWindow::rebuildDrivers() {
    if (!driversList) return;
    driversList->clear();
    for (int id : {1, 2, 3}) {
        auto d = sys.getDriver(id);
        if (!d) continue;

        QWidget* row = new QWidget;
        row->setStyleSheet(
            "QWidget { background:#FFFFFF; border:1px solid #E2E2E2; border-radius:10px; }"
            "QWidget:hover { background:#F8F8F8; }");
        QHBoxLayout* hl = new QHBoxLayout(row);
        hl->setContentsMargins(16, 14, 16, 14);
        hl->setSpacing(14);

        // Avatar
        QLabel* av = new QLabel(QString::fromStdString(d->getName()).left(1));
        av->setFixedSize(44, 44);
        av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet(
            "background:#E6F5ED; color:#00A651; border-radius:22px;"
            "font-size:18px; font-weight:700; border:none;");

        QVBoxLayout* info = new QVBoxLayout;
        info->setSpacing(3);
        QLabel* nm = new QLabel(QString::fromStdString(d->getName()));
        nm->setStyleSheet("font-size:14px; font-weight:700; color:#1A1A1A;"
                          "background:transparent; border:none;");
        QLabel* mt = new QLabel(
            QString("Driver #%1  |  Rating: %2  |  %3 trips")
            .arg(d->getDriverID())
            .arg(d->getRating(), 0, 'f', 1)
            .arg(d->rideCount()));
        mt->setStyleSheet("font-size:12px; color:#767676; background:transparent; border:none;");
        info->addWidget(nm);
        info->addWidget(mt);

        QLabel* earned = new QLabel(f2(d->totalEarnings()));
        earned->setStyleSheet(
            "font-size:18px; font-weight:700; color:#00A651;"
            "background:transparent; border:none;");
        earned->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        hl->addWidget(av);
        hl->addLayout(info, 1);
        hl->addWidget(earned);

        QListWidgetItem* item = new QListWidgetItem(driversList);
        item->setSizeHint(QSize(0, 76));
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        driversList->setItemWidget(item, row);
    }
}

void MainWindow::rebuildRiders() {
    if (!ridersList) return;
    ridersList->clear();
    for (int id : {101, 102, 103}) {
        auto r = sys.getRider(id);
        if (!r) continue;

        QWidget* row = new QWidget;
        row->setStyleSheet(
            "QWidget { background:#FFFFFF; border:1px solid #E2E2E2; border-radius:10px; }"
            "QWidget:hover { background:#F8F8F8; }");
        QHBoxLayout* hl = new QHBoxLayout(row);
        hl->setContentsMargins(16, 14, 16, 14);
        hl->setSpacing(14);

        QLabel* av = new QLabel(QString::fromStdString(r->getName()).left(1));
        av->setFixedSize(44, 44);
        av->setAlignment(Qt::AlignCenter);
        av->setStyleSheet(
            "background:#EBF0FE; color:#276EF1; border-radius:22px;"
            "font-size:18px; font-weight:700; border:none;");

        QVBoxLayout* info = new QVBoxLayout;
        info->setSpacing(3);
        QLabel* nm = new QLabel(QString::fromStdString(r->getName()));
        nm->setStyleSheet("font-size:14px; font-weight:700; color:#1A1A1A;"
                          "background:transparent; border:none;");
        QLabel* mt = new QLabel(
            QString("Rider #%1  |  %2 trips").arg(r->getRiderID()).arg(r->rideCount()));
        mt->setStyleSheet("font-size:12px; color:#767676; background:transparent; border:none;");
        info->addWidget(nm);
        info->addWidget(mt);

        QLabel* spent = new QLabel(f2(r->totalSpent()) + " spent");
        spent->setStyleSheet(
            "font-size:14px; font-weight:700; color:#276EF1;"
            "background:transparent; border:none;");
        spent->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        hl->addWidget(av);
        hl->addLayout(info, 1);
        hl->addWidget(spent);

        QListWidgetItem* item = new QListWidgetItem(ridersList);
        item->setSizeHint(QSize(0, 76));
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        ridersList->setItemWidget(item, row);
    }
}

void MainWindow::rebuildAll() {
    rebuildTrips(dashTripsList);
    rebuildTrips(bookTripsList);
    rebuildTrips(allTripsList);
    rebuildDrivers();
    rebuildRiders();
}

void MainWindow::updateMetrics() {
    const auto& rides = sys.getAllRides();
    int n = (int)rides.size();
    double total = 0.0, miles = 0.0;
    for (const auto& r : rides) { total += r->fare(); miles += r->getDistance(); }
    double avg = n ? total / n : 0.0;

    lblTotalRides->setText(QString::number(n));
    lblRevenue->setText(f2(total));
    lblRevenue->setStyleSheet("font-size:28px; font-weight:700; color:#00A651;");
    lblAvgFare->setText(f2(avg));
    lblMiles->setText(QString::asprintf("%.1f", miles));
    lblMiles->setStyleSheet("font-size:28px; font-weight:700; color:#276EF1;");
}

void MainWindow::logMsg(const QString& msg) {
    if (!activityLog) return;
    QString ts = QDateTime::currentDateTime().toString("hh:mm:ss");
    activityLog->append(QString("[%1]  %2").arg(ts, msg));
}
