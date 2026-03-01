/**
 * Cpp/src/main.cpp
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Entry point for the Employee Schedule Manager (C++).
 *
 * Usage
 * ─────
 *   ./SchedulerApp          →  Qt GUI window
 *   ./SchedulerApp --cli    →  CLI demo (no display required)
 *
 * Build
 * ─────
 *   cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
 *   cmake --build build --parallel
 */

#include <QApplication>
#include <QTextStream>
#include <QStringList>

#include "main_window.h"
#include "scheduler.h"
#include "demo_data.h"

// ── CLI demo ──────────────────────────────────────────────────────────────────
static void runCliDemo()
{
    QTextStream out(stdout);
    out << QString(64, '=') << "\n";
    out << "  Employee Schedule Manager  [C++ / CLI demo]\n";
    out << QString(64, '=') << "\n\n";

    auto employees = makeDemoEmployees();
    out << QString("Loaded %1 employees:\n\n").arg(employees.size());
    for (const auto &emp : employees)
    {
        out << "  " << emp.name.leftJustified(24)
            << " prefs: " << emp.preferences.join(" > ") << "\n";
    }

    Scheduler sched(employees);
    out << "\n[INFO] Building schedule...\n";
    out.flush();
    sched.build();
    out << "[INFO] Done.\n\n";
    out.flush();

    sched.printSchedule();
    sched.printEmployeeSummary();
}

// ── Qt GUI ────────────────────────────────────────────────────────────────────
static int runGui(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Employee Schedule Manager");
    app.setOrganizationName("SchedulerProject");

    MainWindow w;
    w.show();
    return app.exec();
}

// ── main ──────────────────────────────────────────────────────────────────────
int main(int argc, char *argv[])
{
    // Check for --cli flag before constructing QApplication
    for (int i = 1; i < argc; ++i)
    {
        if (QString(argv[i]) == "--cli")
        {
            // QCoreApplication needed for Qt string utilities
            QCoreApplication core(argc, argv);
            runCliDemo();
            return 0;
        }
    }
    return runGui(argc, argv);
}
