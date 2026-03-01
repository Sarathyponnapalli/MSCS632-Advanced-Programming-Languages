/**
 * Cpp/include/demo_data.h
 * ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 * Pre-built Indian employee roster for demo / testing.
 */
#pragma once

#include "employee.h"
#include <QVector>

inline QVector<Employee> makeDemoEmployees()
{
    struct Row
    {
        const char *name;
        QStringList prefs;
    };

    const QVector<Row> data = {
        {"Aarav Sharma", {"morning", "afternoon", "evening"}},
        {"Priya Patel", {"afternoon", "morning", "evening"}},
        {"Rohan Mehta", {"evening", "morning", "afternoon"}},
        {"Divya Nair", {"morning", "evening", "afternoon"}},
        {"Kiran Reddy", {"afternoon", "evening", "morning"}},
        {"Sneha Iyer", {"evening", "afternoon", "morning"}},
        {"Arjun Gupta", {"morning", "afternoon", "evening"}},
        {"Meera Joshi", {"afternoon", "morning", "evening"}},
        {"Vikram Singh", {"evening", "morning", "afternoon"}},
        {"Ananya Pillai", {"morning", "evening", "afternoon"}},
    };

    QVector<Employee> employees;
    employees.reserve(data.size());
    for (const auto &row : data)
        employees.emplace_back(QString(row.name), row.prefs);
    return employees;
}
