/*
 * Copyright (C) Kreogist Dev Team
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
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <QBoxLayout>
#include <QLabel>

#include "sao/knsaosubmenu.h"
#include "knimagelabel.h"
#include "knlabellineedit.h"
#include "knlocalemanager.h"
#include "knopacityanimebutton.h"

#include "knaccountloginpanel.h"

#define ButtonSize 36

KNAccountLoginPanel::KNAccountLoginPanel(QWidget *parent) :
    QWidget(parent),
    m_title(new QLabel(this)),
    m_subTitle(new QLabel(this)),
    m_username(new KNLabelLineEdit(this)),
    m_password(new KNLabelLineEdit(this)),
    m_login(generateButton("://public/ok.png")),
    m_others(generateButton("://public/others.png")),
    m_actionMenu(new KNSaoSubMenu(m_others))
{
    //Set properties.
    setFixedHeight(270);
    //Configure the title label.
    m_title->setAlignment(Qt::AlignCenter);
    QFont titleFont=m_title->font();
    titleFont.setPixelSize(18);
    m_title->setFont(titleFont);
    m_subTitle->setAlignment(Qt::AlignCenter);
    //Configure the user name line edit.
    m_username->setMinimumLightness(0xC0);
    m_username->setMediumLightness(0xE0);
    //Configure password line edit.
    m_password->setMinimumLightness(0xC0);
    m_password->setMediumLightness(0xE0);
    m_password->setEchoMode(QLineEdit::Password);
    //Initial the action.
    for(int i=0; i<AccountActionCount; ++i)
    {
        //Initial the action.
        m_menuActions[i]=new QAction(this);
        //Add action to menu.
        m_actionMenu->addAction(m_menuActions[i]);
    }
    //Set the icon to the actions.
    m_menuActions[RegisterAccount]->setIcon(QIcon("://saomenuicons/user.png"));
    m_menuActions[ForgetPassword]->setIcon(
                QIcon("://saomenuicons/question.png"));
    //Link the actions.
    connect(m_menuActions[RegisterAccount], SIGNAL(triggered(bool)),
            this, SIGNAL(requireRegister()));
    //Link the other button.
    connect(m_others, &KNOpacityAnimeButton::clicked,
            [=]
            {
                //Show the action menu.
                m_actionMenu->exec();
            });

    //Initial the box layout.
    QBoxLayout *mainLayout=new QBoxLayout(QBoxLayout::TopToBottom, this);
    setLayout(mainLayout);
    //Initial the label.
    KNImageLabel *iconLabel=new KNImageLabel(this);
    iconLabel->setFixedSize(79, 79);
    iconLabel->setPixmap(QPixmap(":/public/Kreogist.png"));
    //Add widget to layout
    mainLayout->addSpacing(5);
    mainLayout->addWidget(m_title);
    mainLayout->addSpacing(2);
    mainLayout->addWidget(m_subTitle);
    mainLayout->addSpacing(7);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(7);
    mainLayout->addWidget(m_username);
    mainLayout->addWidget(m_password);
    //Initial button layout.
    QBoxLayout *buttonLayout=new QBoxLayout(QBoxLayout::LeftToRight,
                                            mainLayout->widget());
    mainLayout->addSpacing(8);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    //Add all buttons.
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_login);
    buttonLayout->addSpacing(45);
    buttonLayout->addWidget(m_others);
    buttonLayout->addStretch();

    //Link the retranslator.
    knI18n->link(this, &KNAccountLoginPanel::retranslate);
    retranslate();
}

void KNAccountLoginPanel::hideEvent(QHideEvent *event)
{
    //Hide the widget first.
    QWidget::hideEvent(event);
    //CLear the data.
    m_username->clear();
    m_password->clear();
}

void KNAccountLoginPanel::retranslate()
{
    //Update label.
    m_title->setText(tr("Sign in"));
    m_subTitle->setText(tr("with Your Kreogist Account"));
    //Set the place holder text.
    m_username->setPlaceholderText(tr("Username/E-mail"));
    m_password->setPlaceholderText(tr("Password"));
    //Initial the actions.
    m_menuActions[RegisterAccount]->setText(tr("Create an account"));
    m_menuActions[ForgetPassword]->setText(tr("Forget password?"));
}

void KNAccountLoginPanel::onActionForgetPassword()
{
    ;
}

inline KNOpacityAnimeButton *KNAccountLoginPanel::generateButton(
        const QString &iconPath)
{
    //Initial the button.
    KNOpacityAnimeButton *button=new KNOpacityAnimeButton(this);
    //Configure the button.
    button->setIcon(QIcon(iconPath));
    button->setFixedSize(ButtonSize, ButtonSize);
    //Give back the button.
    return button;
}