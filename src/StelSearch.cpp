/*
 * Stellarium
 * Copyright (C) 2008 Guillaume Chereau
 *
 * Stellarium N9 QML Gui
 * Copyright (C) 2012 Marko Srebre
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
*/

#include "StelSearch.hpp"
#include "StelApp.hpp"
#include "StelCore.hpp"
#include "StelModuleMgr.hpp"
#include "StelMovementMgr.hpp"
#include "StelTranslator.hpp"

#include "StelObjectMgr.hpp"
#include "StelUtils.hpp"

#include <QDebug>
#include <QSettings>
#include <QString>
#include <QStringList>



StelSearch::StelSearch(QObject* parent) :
    QObject(parent)
{
    greekLetters.insert("alpha", QString(QChar(0x03B1)));
    greekLetters.insert("beta", QString(QChar(0x03B2)));
    greekLetters.insert("gamma", QString(QChar(0x03B3)));
    greekLetters.insert("delta", QString(QChar(0x03B4)));
    greekLetters.insert("epsilon", QString(QChar(0x03B5)));

    greekLetters.insert("zeta", QString(QChar(0x03B6)));
    greekLetters.insert("eta", QString(QChar(0x03B7)));
    greekLetters.insert("theta", QString(QChar(0x03B8)));
    greekLetters.insert("iota", QString(QChar(0x03B9)));
    greekLetters.insert("kappa", QString(QChar(0x03BA)));

    greekLetters.insert("lambda", QString(QChar(0x03BB)));
    greekLetters.insert("mu", QString(QChar(0x03BC)));
    greekLetters.insert("nu", QString(QChar(0x03BD)));
    greekLetters.insert("xi", QString(QChar(0x03BE)));
    greekLetters.insert("omicron", QString(QChar(0x03BF)));

    greekLetters.insert("pi", QString(QChar(0x03C0)));
    greekLetters.insert("rho", QString(QChar(0x03C1)));
    greekLetters.insert("sigma", QString(QChar(0x03C3))); // second lower-case sigma shouldn't affect anything
    greekLetters.insert("tau", QString(QChar(0x03C4)));
    greekLetters.insert("upsilon", QString(QChar(0x03C5)));

    greekLetters.insert("phi", QString(QChar(0x03C6)));
    greekLetters.insert("chi", QString(QChar(0x03C7)));
    greekLetters.insert("psi", QString(QChar(0x03C8)));
    greekLetters.insert("omega", QString(QChar(0x03C9)));

    QSettings* conf = StelApp::getInstance().getSettings();
    Q_ASSERT(conf);
}

void StelSearch::updateSearchResults(const QString& text, QStringList& matches)
{
    StelObjectMgr* objectMgr = GETSTELMODULE(StelObjectMgr);

    QString trimmedText = text.trimmed().toLower();

    const bool useStartOfWords = true; // TODO: property?

    if (!trimmedText.isEmpty())
    {

        QString greekText = substituteGreek(trimmedText);

        if(greekText != trimmedText) {
            matches = objectMgr->listMatchingObjectsI18n(trimmedText, 6, useStartOfWords);
            matches += objectMgr->listMatchingObjects(trimmedText, 6, useStartOfWords);
            matches += objectMgr->listMatchingObjectsI18n(greekText, (20 - matches.size()), useStartOfWords);
        } else {
            matches = objectMgr->listMatchingObjectsI18n(trimmedText, 20, useStartOfWords);
            matches += objectMgr->listMatchingObjects(trimmedText, 20, useStartOfWords);
        }

        matches.removeDuplicates();
    }

}




bool StelSearch::gotoObject(const QString& name)
{
    if (name.isEmpty())
        return false;

    StelObjectMgr* objectMgr = GETSTELMODULE(StelObjectMgr);
    if (objectMgr->findAndSelectI18n(name) || objectMgr->findAndSelect(name))
    {
        const QList<StelObjectP> newSelected = objectMgr->getSelectedObject();
        if (!newSelected.empty())
        {
            if (newSelected[0]->getEnglishName()!=StelApp::getInstance().getCore()->getCurrentLocation().planetName)
            {
                StelMovementMgr* mvmgr = GETSTELMODULE(StelMovementMgr);
                mvmgr->moveToObject(newSelected[0], mvmgr->getAutoMoveDuration());
                mvmgr->setFlagTracking(true);
            }
            else
            {
                GETSTELMODULE(StelObjectMgr)->unSelect();
            }
        }
        return true;
    }
    return false;
}

bool StelSearch::gotoNamedObject(const QString& text, bool recheck)
{
    QString search_text;
    if (recheck)
    {
        QStringList matches;
        updateSearchResults(text, matches);

        if (matches.count() > 0)
           search_text = matches[0];
    }
    else
    {
        search_text = text;
    }

    return gotoObject(search_text);
}

QString StelSearch::substituteGreek(const QString& keyString)
{
    if (!keyString.contains(' '))
        return getGreekLetterByName(keyString);
    else
    {
        QStringList nameComponents = keyString.split(" ", QString::SkipEmptyParts);
        if(!nameComponents.empty())
            nameComponents[0] = getGreekLetterByName(nameComponents[0]);
        return nameComponents.join(" ");
    }
}

QString StelSearch::getGreekLetterByName(const QString& potentialGreekLetterName)
{
    if(greekLetters.contains(potentialGreekLetterName))
        return greekLetters[potentialGreekLetterName.toLower()];

    // There can be indices (e.g. "α1 Cen" instead of "α Cen A"), so strip
    // any trailing digit.
    int lastCharacterIndex = potentialGreekLetterName.length()-1;
    if(potentialGreekLetterName.at(lastCharacterIndex).isDigit())
    {
        QChar digit = potentialGreekLetterName.at(lastCharacterIndex);
        QString name = potentialGreekLetterName.left(lastCharacterIndex);
        if(greekLetters.contains(name))
            return greekLetters[name.toLower()] + digit;
    }

    return potentialGreekLetterName;
}

