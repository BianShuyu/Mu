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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include <QFile>

#include "knmusicplaylistloader.h"
#include "knmusicplaylistlistitem.h"
#include "knmusicplaylistlistassistant.h"
#include "knmusicplaylisttab.h"
#include "knmusicplaylistlist.h"
#include "knmusicplaylistmodel.h"
#include "knmusicnowplayingbase.h"

#include "knmusicglobal.h"

#include "knmusicplaylistmanager.h"

#include <QDebug>

KNMusicPlaylistManager::KNMusicPlaylistManager(QObject *parent) :
    KNMusicPlaylistManagerBase(parent)
{
    //Set the playlist icon.
    KNMusicPlaylistListAssistant::setPlaylistIcon(
                QIcon(":/plugin/music/playlist/playlist.png"));
    //Set the playlist folder path.
    KNMusicPlaylistListAssistant::setPlaylistFolderPath(
                KNMusicGlobal::musicLibraryPath()+"/Playlist");
    //Initial the default database path.
    m_playlistDatabasePath=KNMusicPlaylistListAssistant::playlistFolderPath() +
                           "/Playlist.mudb";

    //Initial the playlist loader.
    initialPlaylistLoader();
    //Initial playlist viewer UI.
    m_playlistTab=new KNMusicPlaylistTab(this);
    //Generate the playlist list.
    m_playlistList=new KNMusicPlaylistList(this);
    //Set the playlist list.
    m_playlistTab->setPlaylistList(m_playlistList);

    //Link the UI's request.
    connect(m_playlistTab, &KNMusicPlaylistTab::requireGeneratePlaylist,
            this, &KNMusicPlaylistManager::onActionAddPlaylist);
    connect(m_playlistTab, &KNMusicPlaylistTab::requireRemovePlaylist,
            this, &KNMusicPlaylistManager::onActionRemovePlaylist);
    connect(m_playlistTab, &KNMusicPlaylistTab::currentPlaylistChanged,
            this, &KNMusicPlaylistManager::onActionCurrentPlaylistChanged);
    //When the data of playlist list has been changed, update the detail.
    connect(m_playlistList, &KNMusicPlaylistList::itemChanged,
            m_playlistTab, &KNMusicPlaylistTab::onActionPlaylistItemChanged);

    //Temporary load here.
    loadPlaylistList();
}

KNMusicPlaylistManager::~KNMusicPlaylistManager()
{
    //Save all the playlist changes first.
    saveChangedPlaylist();
    //Save the playlist list data.
    KNMusicPlaylistListAssistant::savePlaylistDatabase(m_playlistDatabasePath,
                                                       m_playlistList->playlistListData());
}

KNMusicTab *KNMusicPlaylistManager::categoryTab()
{
    return m_playlistTab;
}

void KNMusicPlaylistManager::loadPlaylistList()
{
    //Initial a blank playlist files.
    QStringList rawFiles, playlistFiles;
    //Load the raw playlist files.
    KNMusicPlaylistListAssistant::loadPlaylistDatabase(m_playlistDatabasePath,
                                                       rawFiles);
    //Load all the files.
    while(!rawFiles.isEmpty())
    {
        QString currentFile=rawFiles.takeFirst();
        //If we can load this file, add to the playlist list.
        if(importPlaylistFromFile(currentFile))
        {
            playlistFiles.append(currentFile);
        }
    }
    //Set the playlist list data to the list.
    m_playlistList->setPlaylistListData(playlistFiles);
}

void KNMusicPlaylistManager::onActionAddPlaylist(const QString &caption)
{
    KNMusicPlaylistListItem *playlistItem=
            KNMusicPlaylistListAssistant::generateBlankPlaylist(caption);
    m_playlistList->appendRow(playlistItem);
    //Set the new playlist to the current playlist.
    m_playlistTab->setCurrentPlaylist(playlistItem->index());
    //Let user rename it automatically.
    m_playlistTab->editPlaylistName(playlistItem->index());
}

void KNMusicPlaylistManager::onActionRemovePlaylist(const QModelIndex &index)
{
    int playlistItemRow=index.row();
    //Try to remove the item's file.
    QFile playlistFile(m_playlistList->playlistItem(playlistItemRow)->playlistFilePath());
    if(playlistFile.exists())
    {
        //Remove the playlist file.
        if(!playlistFile.remove())
        {
            //!FIXME: We need to tell user that we cannot remove the file.
            return;
        }
    }
    //Ask now playing to check the model.
    KNMusicGlobal::nowPlaying()->checkRemovedModel(
                m_playlistList->playlistModel(playlistItemRow));
    //Remove that row.
    m_playlistList->removeRow(playlistItemRow);
}

void KNMusicPlaylistManager::onActionCurrentPlaylistChanged(const QModelIndex &current,
                                                            const QModelIndex &previous)
{
    Q_UNUSED(previous)
    //Ask UI to display the playlist item.
    m_playlistTab->displayPlaylistItem(m_playlistList->playlistItemFromIndex(current));
}

void KNMusicPlaylistManager::initialPlaylistLoader()
{
    //Initial the loader.
    m_loader=new KNMusicPlaylistLoader(this);
    //Install all the plugins.
}

void KNMusicPlaylistManager::saveChangedPlaylist()
{
    for(int i=0; i<m_playlistList->rowCount(); i++)
    {
        KNMusicPlaylistListItem *currentItem=m_playlistList->playlistItem(i);
        if(currentItem->changed())
        {
            KNMusicPlaylistListAssistant::writePlaylist(currentItem);
        }
    }
}

bool KNMusicPlaylistManager::importPlaylistFromFile(const QString &filePath)
{
    KNMusicPlaylistListItem *playlistItem=
            KNMusicPlaylistListAssistant::generatePlaylist();
    //Using the mu playlist parser first to parse it.
    if(KNMusicPlaylistListAssistant::readPlaylist(filePath, playlistItem))
    {
        //If we can parse it, means it's a standard playlist.
        m_playlistList->appendRow(playlistItem);
        return true;
    }
//    m_loader->parsePlaylist(filePath);
    delete playlistItem;
    return false;
}