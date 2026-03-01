/**
 * Cpp/include/scheduler.h
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Two-pass scheduling algorithm declaration.
 *
 * FIX (build error):
 *   The previous version returned QVector<QMap<QString,QVariant>> from
 *   getEmployeeSummary(). QVariant is only forward-declared when QMap is
 *   included via employee.h, causing "incomplete type 'class QVariant'"
 *   errors on Qt 6.10 / GCC 15.
 *
 *   Solution: introduce a plain EmployeeSummaryRow struct with strongly-
 *   typed fields. No QVariant anywhere — no incomplete-type errors.
 */
#pragma once

#include "employee.h"

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QRandomGenerator>

// ── Summary row returned by getEmployeeSummary() ─────────────────────────────
// Strongly-typed struct replaces QMap<QString,QVariant> to avoid the
// QVariant incomplete-type problem with Qt 6.x + GCC 15.
struct EmployeeSummaryRow
{
    QString name;
    int daysWorked{0};
    QStringList preferences;       ///< ranked: index 0 = most preferred
    QMap<QString, QString> dayMap; ///< { dayName → shiftName }
};

/**
 * @brief Builds the weekly shift schedule for a pool of employees.
 *
 * Two-pass algorithm:
 * ───────────────────
 * Pass 1 (assignPreferredShifts):
 *   For each day, iterate over employees in random order.
 *   Assign the employee's top-ranked shift that respects:
 *     • isAvailable()   — 5-day weekly cap
 *     • !isWorkingOn(d) — one shift per day
 *
 * Pass 2 (fillGaps):
 *   Scan every (day, shift) slot. If headcount < MIN_EMPLOYEES_PER_SHIFT,
 *   pull from the pool of still-available employees sorted by preference
 *   rank for that shift (bonus: best-match person chosen first).
 *
 * NOTE: No Q_OBJECT — pure logic class, no MOC needed.
 *       The Qt GUI class (MainWindow) lives in main_window.h/.cpp.
 */
class Scheduler
{
public:
    // schedule[dayIdx][shiftIdx] = list of employee names
    QVector<QVector<QStringList>> schedule;
    QStringList warnings;

    explicit Scheduler(QVector<Employee> &employees);

    void build();

    // ── Reporting (consumed by both Qt GUI and CLI) ───────────────────────────
    QVector<EmployeeSummaryRow> getEmployeeSummary() const;

    // CLI text output
    void printSchedule() const;
    void printEmployeeSummary() const;

private:
    QVector<Employee> &employees_;

    void assignPreferredShifts();
    void fillGaps();
    void doAssign(Employee &emp, int dayIdx, const QString &shift);
    void reset();
};
