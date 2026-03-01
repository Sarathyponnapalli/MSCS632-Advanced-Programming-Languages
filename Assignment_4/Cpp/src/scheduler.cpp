/**
 * Cpp/src/scheduler.cpp
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Scheduler implementation — two-pass algorithm.
 *
 * FIX (build error):
 *   getEmployeeSummary() now returns QVector<EmployeeSummaryRow>
 *   (a typed struct defined in scheduler.h) instead of
 *   QVector<QMap<QString,QVariant>>.  This eliminates all
 *   "invalid use of incomplete type 'class QVariant'" errors that
 *   appeared with Qt 6.10 + GCC 15 because QVariant was only
 *   forward-declared when pulled in through QMap's include chain.
 */

#include "scheduler.h"

#include <QTextStream>
#include <algorithm>
#include <numeric>

// ── Constructor ───────────────────────────────────────────────────────────────
Scheduler::Scheduler(QVector<Employee> &employees)
    : schedule(NUM_DAYS, QVector<QStringList>(NUM_SHIFTS)), employees_(employees)
{
}

// ── Public: build ─────────────────────────────────────────────────────────────
void Scheduler::build()
{
    reset();
    assignPreferredShifts();
    fillGaps();
}

// ── Pass 1: preferred assignment ──────────────────────────────────────────────
void Scheduler::assignPreferredShifts()
{
    for (int d = 0; d < NUM_DAYS; ++d)
    {
        // Shuffle employee indices each day for fairness (no fixed priority)
        QVector<int> idx(employees_.size());
        std::iota(idx.begin(), idx.end(), 0);

        for (int i = idx.size() - 1; i > 0; --i)
        {
            int j = static_cast<int>(
                QRandomGenerator::global()->bounded(static_cast<quint32>(i + 1)));
            std::swap(idx[i], idx[j]);
        }

        for (int k : idx)
        {
            Employee &emp = employees_[k];
            if (!emp.isAvailable())
                continue;
            if (emp.isWorkingOn(d))
                continue;

            // Walk the ranked preference list; assign the first valid shift.
            // Conflict resolution: if the employee is capped (isAvailable)
            // or already placed today (isWorkingOn), the guards skip them.
            // Any remaining unassigned employees are picked up in Pass 2.
            for (const QString &shift : emp.preferences)
            {
                doAssign(emp, d, shift);
                break; // assigned — move to next employee
            }
        }
    }
}

// ── Pass 2: fill gaps ─────────────────────────────────────────────────────────
void Scheduler::fillGaps()
{
    for (int d = 0; d < NUM_DAYS; ++d)
    {
        for (int s = 0; s < NUM_SHIFTS; ++s)
        {
            while (schedule[d][s].size() < MIN_EMPLOYEES_PER_SHIFT)
            {
                // Collect employees not yet placed today and still available
                QVector<int> available;
                for (int i = 0; i < employees_.size(); ++i)
                {
                    const Employee &e = employees_[i];
                    if (!e.isWorkingOn(d) && e.isAvailable())
                        available.append(i);
                }

                if (available.isEmpty())
                {
                    warnings << QString("Cannot fill '%1' on %2: "
                                        "not enough available employees.")
                                    .arg(SHIFTS[s])
                                    .arg(DAYS[d]);
                    break;
                }

                // Bonus: sort by preference rank so the best-matched
                // employee is always chosen first even in forced assignments.
                std::sort(available.begin(), available.end(),
                          [&](int a, int b)
                          {
                              return employees_[a].preferenceRank(SHIFTS[s]) <
                                     employees_[b].preferenceRank(SHIFTS[s]);
                          });

                doAssign(employees_[available[0]], d, SHIFTS[s]);
            }
        }
    }
}

// ── Helper: assign + record ───────────────────────────────────────────────────
void Scheduler::doAssign(Employee &emp, int dayIdx, const QString &shift)
{
    emp.assign(dayIdx, shift);
    int sIdx = indexOfStr(SHIFTS, shift);
    if (sIdx >= 0)
        schedule[dayIdx][sIdx] << emp.name;
}

// ── Reset ─────────────────────────────────────────────────────────────────────
void Scheduler::reset()
{
    for (auto &e : employees_)
        e.reset();
    schedule = QVector<QVector<QStringList>>(
        NUM_DAYS, QVector<QStringList>(NUM_SHIFTS));
    warnings.clear();
}

// ── Reporting: typed summary (NO QVariant) ────────────────────────────────────
QVector<EmployeeSummaryRow> Scheduler::getEmployeeSummary() const
{
    // Collect const pointers, then sort by name
    QVector<const Employee *> sorted;
    sorted.reserve(employees_.size());
    for (const auto &e : employees_)
        sorted << &e;

    std::sort(sorted.begin(), sorted.end(),
              [](const Employee *a, const Employee *b)
              { return a->name < b->name; });

    QVector<EmployeeSummaryRow> out;
    out.reserve(sorted.size());

    for (const Employee *ep : sorted)
    {
        EmployeeSummaryRow row;
        row.name = ep->name;
        row.daysWorked = ep->daysWorked;
        row.preferences = ep->preferences;

        // Build per-day map: { "Monday" -> "morning", … }
        for (int d = 0; d < NUM_DAYS; ++d)
            if (ep->isWorkingOn(d))
                row.dayMap[DAYS[d]] = ep->schedule[d];

        out << row;
    }
    return out;
}

// ── CLI: print weekly schedule ────────────────────────────────────────────────
void Scheduler::printSchedule() const
{
    QTextStream out(stdout);
    const int COL = 26;
    const int DCOL = 13;
    const int total = DCOL + COL * NUM_SHIFTS;

    const QString dbl(total, '=');
    const QString sep(total, '-');

    out << dbl << "\n";
    out << "         WEEKLY EMPLOYEE SCHEDULE\n";
    out << dbl << "\n";

    // Header row
    out << QString("Day").leftJustified(DCOL);
    for (const auto &s : SHIFTS)
        out << QString(s[0].toUpper() + s.mid(1)).leftJustified(COL);
    out << "\n"
        << sep << "\n";

    // Data rows
    for (int d = 0; d < NUM_DAYS; ++d)
    {
        out << DAYS[d].leftJustified(DCOL);
        for (int s = 0; s < NUM_SHIFTS; ++s)
        {
            QString cell = schedule[d][s].isEmpty()
                               ? "(empty)"
                               : schedule[d][s].join(", ");
            out << cell.leftJustified(COL);
        }
        out << "\n";
    }
    out << dbl << "\n";

    if (!warnings.isEmpty())
    {
        out << "\n[WARNINGS]\n";
        for (const auto &w : warnings)
            out << "  ⚠  " << w << "\n";
    }
    out.flush();
}

// ── CLI: print employee summary ───────────────────────────────────────────────
void Scheduler::printEmployeeSummary() const
{
    QTextStream out(stdout);
    const QString STARS[] = {"★★★", "★★☆", "★☆☆"};

    out << "\n         EMPLOYEE SUMMARY\n";
    out << QString(56, '-') << "\n";

    const auto summary = getEmployeeSummary();
    for (const auto &row : summary)
    {
        out << "  " << row.name.leftJustified(24)
            << "Days worked: " << row.daysWorked << "\n";

        for (const QString &day : DAYS)
        {
            if (row.dayMap.contains(day))
            {
                const QString &shift = row.dayMap[day];
                int rank = row.preferences.indexOf(shift);
                QString stars = (rank >= 0 && rank < 3) ? STARS[rank] : "?";
                out << "    " << day.leftJustified(13)
                    << " -> " << shift.leftJustified(12) << stars << "\n";
            }
        }
    }
    out << QString(56, '-') << "\n";
    out.flush();
}
