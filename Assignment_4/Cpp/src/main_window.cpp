/**
 * Cpp/src/main_window.cpp
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Qt GUI implementation.  The layout is compiled from:
 *   shared/ui/main_window.ui
 * via AUTOUIC → ui_main_window.h (see CMakeLists.txt).
 *
 * FIX (build error):
 *   populateSummaryTab() previously called .toString() / .toInt() /
 *   .toStringList() / .value<>() on QVariant map values, which failed
 *   because QVariant was incomplete at that point.
 *   Now it uses the strongly-typed EmployeeSummaryRow struct fields
 *   (name, daysWorked, preferences, dayMap) directly.
 */

#include "main_window.h"
#include "demo_data.h"
#include "ui_main_window.h"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QColor>
#include <QFont>
#include <QBrush>
#include <QSet>
#include <QList>

// ── Colour palette ────────────────────────────────────────────────────────────
static const QMap<QString, QColor> SHIFT_BG = {
    {"morning", QColor("#FFF3CD")},
    {"afternoon", QColor("#D1ECF1")},
    {"evening", QColor("#E2D9F3")},
};
// Dark contrasting foreground for each pastel background
static const QMap<QString, QColor> SHIFT_FG = {
    {"morning", QColor("#6D4C00")},   // dark amber  on #FFF3CD
    {"afternoon", QColor("#0A4550")}, // dark teal   on #D1ECF1
    {"evening", QColor("#3D1A52")},   // dark violet on #E2D9F3
};
static const QMap<QString, QString> SHIFT_EMOJI = {
    {"morning", "🌅"},
    {"afternoon", "☀️"},
    {"evening", "🌙"},
};
static const QColor EMPTY_BG("#F8D7DA");
static const QColor EMPTY_FG("#721C24");
static const QColor DARK_TEXT("#1A252F");
static const QColor DIM_TEXT("#888888");
static const QString STARS[] = {"★★★", "★★☆", "★☆☆"};

// ── makeItem — always sets an explicit foreground so palette can't override ───
static QTableWidgetItem *makeItem(
    const QString &text,
    const QColor &bg = QColor(),
    const QColor &fg = DARK_TEXT,
    Qt::Alignment align = Qt::AlignVCenter | Qt::AlignLeft)
{
    auto *item = new QTableWidgetItem(text);
    item->setForeground(QBrush(fg));
    if (bg.isValid())
        item->setBackground(QBrush(bg));
    item->setTextAlignment(align);
    return item;
}

// ── Constructor / Destructor ──────────────────────────────────────────────────
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    applyStylesheet();
    configureTableHeaders();

    connect(ui->btnAddEmployee, &QPushButton::clicked, this, &MainWindow::onAddEmployee);
    connect(ui->btnLoadDemo, &QPushButton::clicked, this, &MainWindow::onLoadDemo);
    connect(ui->btnClearAll, &QPushButton::clicked, this, &MainWindow::onClearAll);
    connect(ui->btnRemoveEmployee, &QPushButton::clicked, this, &MainWindow::onRemoveEmployee);
    connect(ui->btnBuildSchedule, &QPushButton::clicked, this, &MainWindow::onBuildSchedule);
    connect(ui->actionLoadDemo, &QAction::triggered, this, &MainWindow::onLoadDemo);
    connect(ui->actionExit, &QAction::triggered, this, &QMainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);

    setStatus("Ready — add employees and press ⚙ Build Schedule.");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete scheduler_;
}

// ── Stylesheet ────────────────────────────────────────────────────────────────
void MainWindow::applyStylesheet()
{
    setStyleSheet(R"(
        QMainWindow { background: #F8F9FA; }

        QLabel#lblBanner {
            background: #1A252F;
            color: #FFFFFF;
            font-size: 20px;
            font-weight: bold;
            padding: 10px;
            letter-spacing: 1px;
        }

        QTabWidget::pane {
            border: 1px solid #CED4DA;
            border-radius: 4px;
            background: #FFFFFF;
        }
        QTabBar::tab {
            background: #E9ECEF;
            color: #333333;
            padding: 8px 22px;
            font-size: 13px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
        }
        QTabBar::tab:selected {
            background: #FFFFFF;
            color: #1A252F;
            font-weight: bold;
            border-bottom: 3px solid #2980B9;
        }

        QPushButton {
            background: #2980B9;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 6px 14px;
            font-size: 13px;
        }
        QPushButton:hover   { background: #2471A3; }
        QPushButton:pressed { background: #1A5276; }
        QPushButton#btnRemoveEmployee       { background: #C0392B; }
        QPushButton#btnRemoveEmployee:hover { background: #A93226; }
        QPushButton#btnLoadDemo             { background: #1E8449; }
        QPushButton#btnLoadDemo:hover       { background: #196F3D; }
        QPushButton#btnClearAll             { background: #7F8C8D; }
        QPushButton#btnClearAll:hover       { background: #717D7E; }
        QPushButton#btnBuildSchedule {
            background: #7D3C98;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton#btnBuildSchedule:hover  { background: #6C3483; }

        QGroupBox {
            font-size: 13px;
            font-weight: bold;
            color: #1A252F;
            border: 1px solid #CED4DA;
            border-radius: 6px;
            margin-top: 12px;
            padding-top: 14px;
        }
        QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }

        /* KEY FIX: force cell text colour at stylesheet level as well */
        QTableWidget {
            border: 1px solid #DEE2E6;
            border-radius: 4px;
            gridline-color: #DEE2E6;
            font-size: 12px;
            color: #1A252F;
        }
        QTableWidget::item {
            color: #1A252F;
            padding: 2px 4px;
        }
        QTableWidget::item:selected {
            background: #AED6F1;
            color: #1A252F;
        }
        QHeaderView::section {
            background: #1A252F;
            color: #FFFFFF;
            padding: 6px;
            font-size: 12px;
            font-weight: bold;
            border: none;
        }

        QListWidget {
            border: 2px dashed #ADB5BD;
            border-radius: 4px;
            font-size: 13px;
            color: #1A252F;
            background: #FFFFFF;
        }
        QListWidget::item {
            padding: 7px 10px;
            border-bottom: 1px solid #DEE2E6;
            color: #1A252F;
        }
        QListWidget::item:selected { background: #D6EAF8; color: #1A252F; }

        QLineEdit {
            border: 1px solid #CED4DA;
            border-radius: 4px;
            padding: 6px;
            font-size: 13px;
            color: #1A252F;
            background: #FFFFFF;
        }
        QLineEdit:focus { border: 2px solid #2980B9; }

        QLabel#lblStatus {
            background: #ECF0F1;
            color: #555555;
            padding: 4px 12px;
            font-size: 12px;
            border-top: 1px solid #CED4DA;
        }

        QTextEdit {
            border: 1px solid #CED4DA;
            border-radius: 4px;
            font-size: 12px;
            color: #1A252F;
            background: #FFFFFF;
        }
    )");
}

// ── Table configuration ───────────────────────────────────────────────────────
void MainWindow::configureTableHeaders()
{
    // Employee input table
    auto *eh = ui->tblEmployees->horizontalHeader();
    eh->setSectionResizeMode(0, QHeaderView::Stretch);
    for (int c = 1; c <= 3; ++c)
        eh->setSectionResizeMode(c, QHeaderView::ResizeToContents);
    ui->tblEmployees->verticalHeader()->setDefaultSectionSize(30);

    // Schedule table: 7 days × 3 shifts
    ui->tblSchedule->setRowCount(NUM_DAYS);
    ui->tblSchedule->setColumnCount(NUM_SHIFTS);
    ui->tblSchedule->setVerticalHeaderLabels(DAYS);
    QStringList shiftHdrs;
    for (const auto &s : SHIFTS)
        shiftHdrs << SHIFT_EMOJI[s] + "  " + s[0].toUpper() + s.mid(1);
    ui->tblSchedule->setHorizontalHeaderLabels(shiftHdrs);
    for (int c = 0; c < NUM_SHIFTS; ++c)
        ui->tblSchedule->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
    ui->tblSchedule->verticalHeader()->setDefaultSectionSize(70);

    // Summary table
    auto *sh = ui->tblSummary->horizontalHeader();
    sh->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    sh->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    sh->setSectionResizeMode(2, QHeaderView::Stretch);
    for (int c = 3; c < 10; ++c)
        sh->setSectionResizeMode(c, QHeaderView::ResizeToContents);
}

// ── Slot: add employee ────────────────────────────────────────────────────────
void MainWindow::onAddEmployee()
{
    QString name = ui->leEmployeeName->text().trimmed();
    if (name.isEmpty())
    {
        setStatus("⚠  Please enter an employee name.", true);
        return;
    }

    QStringList prefs = readPreferencesFromList();
    try
    {
        Employee emp(name, prefs);
        for (const auto &e : employees_)
            if (e.name == emp.name)
            {
                setStatus(QString("⚠  '%1' is already registered.").arg(name), true);
                return;
            }
        employees_ << emp;
        refreshEmployeeTable();
        ui->leEmployeeName->clear();
        setStatus(QString("✓  %1 added — 1st: %2, 2nd: %3, 3rd: %4.  Total: %5.")
                      .arg(name)
                      .arg(prefs[0])
                      .arg(prefs[1])
                      .arg(prefs[2])
                      .arg(employees_.size()));
    }
    catch (const std::exception &ex)
    {
        setStatus(QString("⚠  %1").arg(ex.what()), true);
    }
}

void MainWindow::onLoadDemo()
{
    employees_ = makeDemoEmployees();
    refreshEmployeeTable();
    setStatus(QString("✓  Loaded %1 Indian demo employees.").arg(employees_.size()));
}

void MainWindow::onClearAll()
{
    employees_.clear();
    refreshEmployeeTable();
    ui->tblSchedule->clearContents();
    ui->tblSummary->setRowCount(0);
    ui->teWarnings->clear();
    setStatus("All employees cleared.");
}

void MainWindow::onRemoveEmployee()
{
    auto selected = ui->tblEmployees->selectedItems();
    if (selected.isEmpty())
    {
        setStatus("⚠  Select a row first.", true);
        return;
    }
    QSet<int> rowSet;
    for (auto *item : selected)
        rowSet.insert(item->row());
    QList<int> rows = rowSet.values();
    std::sort(rows.begin(), rows.end(), std::greater<int>());
    for (int r : rows)
        employees_.remove(r);
    refreshEmployeeTable();
    setStatus(QString("Removed %1 employee(s).").arg(rowSet.size()));
}

void MainWindow::onBuildSchedule()
{
    if (employees_.size() < 2)
    {
        QMessageBox::warning(this, "Not Enough Employees",
                             "Please add at least 2 employees before building a schedule.");
        return;
    }
    delete scheduler_;
    scheduler_ = new Scheduler(employees_);
    scheduler_->build();
    populateScheduleTab();
    populateSummaryTab();
    populateWarnings();
    ui->tabWidget->setCurrentIndex(1);
    int wc = scheduler_->warnings.size();
    setStatus(QString("✓  Schedule built for %1 employees.  Warnings: %2.")
                  .arg(employees_.size())
                  .arg(wc),
              wc > 0);
}

void MainWindow::onAbout()
{
    QMessageBox::information(this, "About — Employee Schedule Manager",
                             "Employee Schedule Manager\n\n"
                             "C++ Qt6  +  AUTOUIC\n"
                             "Python  +  PySide6 / QUiLoader\n\n"
                             "Both share:  shared/ui/main_window.ui\n\n"
                             "Features:\n"
                             "  • Drag-and-rank shift preferences\n"
                             "  • Two-pass scheduling algorithm\n"
                             "  • Preference-aware gap filling\n"
                             "  • ★ rating per assigned shift\n");
}

// ── Refresh: employee input table ─────────────────────────────────────────────
void MainWindow::refreshEmployeeTable()
{
    ui->tblEmployees->setRowCount(employees_.size());

    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPointSize(11);

    for (int r = 0; r < employees_.size(); ++r)
    {
        const Employee &emp = employees_[r];

        // Col 0 — Name: white background, explicit dark text, bold
        auto *nameItem = makeItem(emp.name, QColor("#FFFFFF"), DARK_TEXT,
                                  Qt::AlignVCenter | Qt::AlignLeft);
        nameItem->setFont(boldFont);
        ui->tblEmployees->setItem(r, 0, nameItem);

        // Cols 1-3 — shift preference cells: pastel BG + dark matching FG
        QFont shiftFont;
        shiftFont.setBold(true);

        for (int c = 0; c < NUM_SHIFTS; ++c)
        {
            const QString &shift = emp.preferences[c];
            QString label = SHIFT_EMOJI[shift] + "  " +
                            shift[0].toUpper() + shift.mid(1);
            auto *cell = makeItem(label, SHIFT_BG[shift], SHIFT_FG[shift],
                                  Qt::AlignCenter);
            cell->setFont(shiftFont);
            ui->tblEmployees->setItem(r, c + 1, cell);
        }
    }
    ui->tblEmployees->resizeRowsToContents();
}

// ── Populate: weekly schedule tab ─────────────────────────────────────────────
void MainWindow::populateScheduleTab()
{
    QFont bold;
    bold.setBold(true);

    for (int d = 0; d < NUM_DAYS; ++d)
    {
        for (int s = 0; s < NUM_SHIFTS; ++s)
        {
            const QStringList &names = scheduler_->schedule[d][s];
            bool empty = names.isEmpty();
            QString text = empty ? "(empty)" : names.join("\n");
            QColor bg = empty ? EMPTY_BG : SHIFT_BG[SHIFTS[s]];
            QColor fg = empty ? EMPTY_FG : SHIFT_FG[SHIFTS[s]];

            auto *item = makeItem(text, bg, fg, Qt::AlignCenter | Qt::AlignVCenter);
            if (empty)
                item->setFont(bold);
            ui->tblSchedule->setItem(d, s, item);
        }
    }
    ui->tblSchedule->resizeRowsToContents();
}

// ── Populate: summary tab ─────────────────────────────────────────────────────
void MainWindow::populateSummaryTab()
{
    const auto summary = scheduler_->getEmployeeSummary();
    ui->tblSummary->setRowCount(summary.size());

    QFont bold;
    bold.setBold(true);

    for (int r = 0; r < summary.size(); ++r)
    {
        const EmployeeSummaryRow &row = summary[r];

        // Name
        auto *nameItem = makeItem(row.name);
        nameItem->setFont(bold);
        ui->tblSummary->setItem(r, 0, nameItem);

        // Days worked
        ui->tblSummary->setItem(r, 1,
                                makeItem(QString::number(row.daysWorked), QColor(), DARK_TEXT, Qt::AlignCenter));

        // Preference chain
        QStringList chain;
        for (const auto &s : row.preferences)
            chain << SHIFT_EMOJI[s] + " " + s[0].toUpper() + s.mid(1);
        ui->tblSummary->setItem(r, 2, makeItem(chain.join("  →  ")));

        // Per-day cells
        for (int d = 0; d < NUM_DAYS; ++d)
        {
            const QString shift = row.dayMap.value(DAYS[d]);
            if (!shift.isEmpty())
            {
                int rank = row.preferences.indexOf(shift);
                QString stars = (rank >= 0 && rank < 3) ? STARS[rank] : "";
                QString text = SHIFT_EMOJI[shift] + " " +
                               shift[0].toUpper() + shift.mid(1) + "\n" + stars;
                ui->tblSummary->setItem(r, 3 + d,
                                        makeItem(text, SHIFT_BG[shift], SHIFT_FG[shift],
                                                 Qt::AlignCenter | Qt::AlignVCenter));
            }
            else
            {
                ui->tblSummary->setItem(r, 3 + d,
                                        makeItem("–", QColor(), DIM_TEXT, Qt::AlignCenter | Qt::AlignVCenter));
            }
        }
    }
    ui->tblSummary->resizeRowsToContents();
}

// ── Populate: warnings ────────────────────────────────────────────────────────
void MainWindow::populateWarnings()
{
    ui->teWarnings->clear();
    if (scheduler_->warnings.isEmpty())
        ui->teWarnings->setPlainText("✓  No warnings — all shifts fully staffed.");
    else
    {
        QStringList lines;
        for (const auto &w : scheduler_->warnings)
            lines << ("⚠  " + w);
        ui->teWarnings->setPlainText(lines.join("\n"));
    }
}

// ── Status strip ──────────────────────────────────────────────────────────────
void MainWindow::setStatus(const QString &msg, bool error)
{
    const QString bg = error ? "#FADBD8" : "#D5F5E3";
    const QString fg = error ? "#922B21" : "#1E8449";
    ui->lblStatus->setStyleSheet(
        QString("background:%1;color:%2;padding:4px 12px;"
                "font-size:12px;border-top:1px solid #CED4DA;")
            .arg(bg)
            .arg(fg));
    ui->lblStatus->setText(msg);
}

// ── Read drag-to-rank list ────────────────────────────────────────────────────
QStringList MainWindow::readPreferencesFromList() const
{
    QStringList prefs;
    for (int i = 0; i < ui->lwPreferences->count(); ++i)
    {
        QString text = ui->lwPreferences->item(i)->text().trimmed();
        prefs << text.split(' ').last().trimmed();
    }
    return prefs;
}
