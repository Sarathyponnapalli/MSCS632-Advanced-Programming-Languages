/**
 * Cpp/include/employee.h
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Employee data-model with ranked shift preferences.
 * Shared constants (DAYS, SHIFTS, caps) are defined here and
 * used by every other translation unit.
 */
#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <stdexcept>
#include <algorithm>

// ── Global constants ──────────────────────────────────────────────────────────
inline const QStringList DAYS = {
    "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
inline const QStringList SHIFTS = {
    "morning", "afternoon", "evening"};
inline constexpr int NUM_DAYS = 7;
inline constexpr int NUM_SHIFTS = 3;
inline constexpr int MAX_DAYS_PER_WEEK = 5;
inline constexpr int MIN_EMPLOYEES_PER_SHIFT = 2;

// ── Helper ────────────────────────────────────────────────────────────────────
inline int indexOfStr(const QStringList &list, const QString &val)
{
    return list.indexOf(val); // returns -1 if not found
}

/**
 * @brief Employee data-model.
 *
 * Scheduling invariants enforced here:
 *  - One shift per day   (assign() throws if the day is already filled)
 *  - Max 5 days per week (isAvailable() returns false after 5 assignments)
 */
struct Employee
{
    QString name;
    QStringList preferences; ///< ranked: index 0 = most preferred
    int daysWorked{0};
    QVector<QString> schedule; ///< schedule[dayIndex] = shift name, or ""

    // ── Constructors ──────────────────────────────────────────────────────────
    Employee() : schedule(NUM_DAYS, QString{}) {}

    Employee(const QString &n, const QStringList &prefs)
        : name(n), preferences(prefs), daysWorked(0), schedule(NUM_DAYS, QString{})
    {
        if (n.trimmed().isEmpty())
            throw std::invalid_argument("Employee name cannot be empty.");
        if (prefs.size() != NUM_SHIFTS)
            throw std::invalid_argument("Exactly 3 shift preferences required.");
        for (const auto &p : prefs)
            if (indexOfStr(SHIFTS, p) < 0)
                throw std::invalid_argument(
                    QString("Unknown shift: '%1'").arg(p).toStdString());
        for (const auto &s : SHIFTS)
            if (!prefs.contains(s))
                throw std::invalid_argument(
                    QString("Shift '%1' missing from preferences.").arg(s).toStdString());
    }

    // ── Queries ───────────────────────────────────────────────────────────────
    bool isAvailable() const { return daysWorked < MAX_DAYS_PER_WEEK; }
    bool isWorkingOn(int d) const { return !schedule[d].isEmpty(); }

    /** 0 = most preferred, NUM_SHIFTS-1 = least preferred, 99 if unknown. */
    int preferenceRank(const QString &shift) const
    {
        int idx = preferences.indexOf(shift);
        return (idx >= 0) ? idx : 99;
    }

    // ── Mutation ──────────────────────────────────────────────────────────────
    void assign(int dayIdx, const QString &shift)
    {
        if (isWorkingOn(dayIdx))
            throw std::runtime_error(
                QString("%1 is already assigned on %2.")
                    .arg(name)
                    .arg(DAYS[dayIdx])
                    .toStdString());
        schedule[dayIdx] = shift;
        ++daysWorked;
    }

    void reset()
    {
        daysWorked = 0;
        schedule.fill(QString{});
    }
};
