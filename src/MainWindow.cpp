#include "MainWindow.h"
#include "ui_mainwindow.h"

#include <typeinfo>
#include <iostream>

#include "SettingManager.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mNetworkMgr(this),
    mParser(0),
    mCurrentSel(0),
    mMoreItems(0),
    mPlayer(0)
{
    ui->setupUi(this);

    mParser = new Parser3( this );

    mParser->NetworkMgr = &mNetworkMgr;
    mParser->ProgressBar = ui->Info_Progress;

    Thumnails.NetworkMgr = &mNetworkMgr;

    {/// Connect all the signals to the slots...
        connect( ui->Search_Button, SIGNAL(clicked()),              SLOT(SearchButton_Clicked()) );
        connect( ui->Search_Input,  SIGNAL(textChanged(QString)),   SLOT(InputChanged(QString)) );
        connect( ui->Search_Input,  SIGNAL(returnPressed()),        SLOT(SearchButton_Clicked()) );

        connect( mParser,   SIGNAL(VideoParsed(VideoPtr)),          SLOT(VideoParsed(VideoPtr)) );
        connect( mParser,   SIGNAL(PlaylistParsed(VideoFeedPtr)),   SLOT(PlaylistParsed(VideoFeedPtr)) );
        connect( mParser,   SIGNAL(UserParsed(UserPtr)),            SLOT(UserParsed(UserPtr)) );

        connect( mParser,   SIGNAL(VideoSearchParsed(VideoFeedPtr)),        SLOT(VideoSearchParsed(VideoFeedPtr)) );
        connect( mParser,   SIGNAL(PlaylistSearchParsed(PlaylistFeedPtr)),  SLOT(PlaylistSearchParsed(PlaylistFeedPtr)) );
        connect( mParser,   SIGNAL(ChannelSearchParsed(ChannelFeedPtr)),    SLOT(ChannelSearchParsed(ChannelFeedPtr)) );

        connect( mParser,   SIGNAL(VideosAdded(VideoFeedPtr)), SLOT(VideosAddded(VideoFeedPtr)) );

        connect( ui->ItemView,      SIGNAL(itemPressed(QListWidgetItem*)), SLOT(selectionChanged_ItemView(QListWidgetItem*)) );
        connect( ui->ItemContent,   SIGNAL(itemPressed(QListWidgetItem*)), SLOT(selectionChanged_ItemContent(QListWidgetItem*)) );
        connect( ui->ItemView,      SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(ItemDoubleClicked(QListWidgetItem*)) );
        connect( ui->ItemContent,   SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(ItemDoubleClicked(QListWidgetItem*)) );

        connect( ui->Play_Button,   SIGNAL(clicked()), SLOT(Play_Clicked()) );
    }

    mPlayer = new PhononYoutubePlayer(0);
    mPlayer->NetworkMgr = &mNetworkMgr;
    mPlayer->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::SearchButton_Clicked()
{
    QString Text = ui->Search_Input->text();

    if( !Text.isEmpty() )// There is no need to do anything if there are no input
    {
        ui->Search_Input->setText("");// Clear the text

        Parser3::UrlType Type = Parser3::getUrlType( QUrl(Text) );

        if( Type == Parser3::UT_Unown ) { // This is probably not a url, treat it as a search
            mParser->Search( Text);
        }
        else { /// This is a url to a video, playlist, user, feed....
            mParser->ParseUrl( QUrl(Text) );
        }
    }
}

void MainWindow::InputChanged( QString Input )
{
    if( Parser3::getUrlType(QUrl(Input)) == Parser3::UT_Unown )
    {
        ui->Search_Button->setText( "Search!" );
    }
    else
    {
        ui->Search_Button->setText( "Go!" );
    }
}

void MainWindow::VideoParsed( VideoPtr Video )
{
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( Video->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );

}

void MainWindow::PlaylistParsed( VideoFeedPtr Playlist )
{
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( Playlist->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );
}

void MainWindow::UserParsed( UserPtr User )
{
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( User->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );
}

void MainWindow::VideoSearchParsed( VideoFeedPtr Search ) {
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( Search->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );
}

void MainWindow::PlaylistSearchParsed( PlaylistFeedPtr Search ) {
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( Search->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );
}

void MainWindow::ChannelSearchParsed( ChannelFeedPtr Search ) {
    QListWidgetItem *Item = new QListWidgetItem( ui->ItemView );

    Item->setSizeHint( QSize(300,64) );

    ListElement *Widget = new ListElement;

    Widget->setInfo( Search->cast_BaseInfo() );
    Widget->setSelected(false);

    ui->ItemView->setItemWidget( Item, Widget );
}

void MainWindow::VideosAddded( VideoFeedPtr Video )
{
    if( Video == mCurContent ) {
        for( int i=ui->ItemContent->count(); i < Video->Content.size(); i++ )
        {
            QListWidgetItem *Item = new QListWidgetItem( ui->ItemContent );
            Item->setSizeHint( QSize(300,64) );
            ListElement *Widget = new ListElement;
            Widget->setInfo( Video->Content.at(i)->cast_BaseInfo() );
            Widget->setSelected(false);
            ui->ItemContent->setItemWidget( Item, Widget );
        }
        if( !Video->HasFullFeed ) {
            /// this can seem like a memory leek but it is not, then ItemContent is cleared this element is also deleted :)
            mMoreItems = new QListWidgetItem( "Double click for more!", ui->ItemContent );
        }
    }
}

void MainWindow::selectionChanged_ItemView( QListWidgetItem *NewItem )
{
    if( !mCurrentSel.isNull() )
    {
        mCurrentSel->setSelected(false);
        mCurrentSel = 0;
    }

    QWidget *Widget = ui->ItemView->itemWidget(NewItem);

    // Just to be sure :P
    if( Widget && (typeid(*Widget) == typeid(ListElement)) )
    {
        mCurrentSel = static_cast<ListElement*>(Widget);
        mCurrentSel->setSelected(true);
        ui->VideoInfo->setText( GenerateRichText_FullInfo(mCurrentSel->getInfo(),
                                                          Settings.getColor(mCurrentSel->getInfo(), SettingManager::CT_NormalText),
                                                          Settings.getColor(mCurrentSel->getInfo(), SettingManager::CT_Heighlighted) ) );


        Thumnails.Download( mCurrentSel->getInfo()->UrlThumbnail, ui->Thumbnail );
    }
}

void MainWindow::selectionChanged_ItemContent( QListWidgetItem *NewItem )
{
    if( !mCurrentSel.isNull() )
    {
        mCurrentSel->setSelected(false);
        mCurrentSel = 0;
    }

    QWidget *Widget = ui->ItemContent->itemWidget(NewItem);

    // Just to be sure :P
    if( Widget && (typeid(*Widget) == typeid(ListElement)) )
    {
        mCurrentSel = static_cast<ListElement*>(Widget);
        mCurrentSel->setSelected(true);
        ui->VideoInfo->setText( GenerateRichText_FullInfo(mCurrentSel->getInfo(),
                                                          Settings.getColor(mCurrentSel->getInfo(), SettingManager::CT_NormalText),
                                                          Settings.getColor(mCurrentSel->getInfo(), SettingManager::CT_Heighlighted) ) );

        Thumnails.Download( mCurrentSel->getInfo()->UrlThumbnail, ui->Thumbnail );
    }
}

void MainWindow::ItemDoubleClicked( QListWidgetItem *Item )
{
    if( Item == mMoreItems )
    {
        if( !mCurContent.isNull() && mCurContent->getType() == IT_VideoFeed )
            mParser->ParseNextPage(  mCurContent->cast_VideoFeed() );
        delete mMoreItems; mMoreItems = 0;
        return;
    }
    QWidget *Widget = Item->listWidget()->itemWidget(Item);
    if( Widget && typeid(*Widget) == typeid(ListElement) )
    {
        BasePtr Info = static_cast<ListElement*>(Widget)->getInfo();

        ui->ItemContent->clear();
        switch( Info->getType() ) {
        case( IT_VideoInfo ):
            {
                QListWidgetItem *Item = new QListWidgetItem( ui->ItemContent );
                Item->setSizeHint( QSize(300,64) );
                ListElement *Widget = new ListElement;
                Widget->setInfo( Info->cast_VideoInfo()->Releated->cast_BaseInfo() );
                Widget->setSelected(false);
                ui->ItemContent->setItemWidget( Item, Widget );

                Item = new QListWidgetItem( ui->ItemContent );
                Item->setSizeHint( QSize(300,64) );
                Widget = new ListElement;
                Widget->setInfo( Info->cast_VideoInfo()->Responces->cast_BaseInfo() );
                Widget->setSelected(false);
                ui->ItemContent->setItemWidget( Item, Widget );
                break;
            }
        case( IT_VideoFeed ): {
            if( Info->cast_VideoFeed()->ParsedPages == 0 ) {
                mParser->ParseNextPage(Info->cast_VideoFeed() );
                mCurContent = Info;
                return;
            }
                VideoFeedPtr fInfo = Info->cast_VideoFeed();
                for( int i=0; i < fInfo->Content.size(); i++ ) {
                    QListWidgetItem *Item = new QListWidgetItem( ui->ItemContent );
                    Item->setSizeHint( QSize(300,64) );
                    ListElement *Widget = new ListElement;
                    Widget->setInfo( fInfo->Content.at(i)->cast_BaseInfo() );
                    Widget->setSelected(false);
                    ui->ItemContent->setItemWidget( Item, Widget );
                }
                if( !fInfo->HasFullFeed ) {
                    /// this can seem like a memory leek but it is not, then ItemContent is cleared this element is also deleted :)
                    mMoreItems = new QListWidgetItem( "Double click for more!", ui->ItemContent );
                }
            break;
        } case( IT_UserInfo ): {
                // Fix this
                UserPtr User = Info->cast_UserInfo();

                QListWidgetItem *Item = 0;
                ListElement *Widget = 0;

                Item = new QListWidgetItem( ui->ItemContent );
                Item->setSizeHint( QSize(300,64) );
                Widget = new ListElement;
                Widget->setInfo( User->Favorites->cast_BaseInfo() );
                Widget->setSelected(false);
                ui->ItemContent->setItemWidget( Item, Widget );

                Item = new QListWidgetItem( ui->ItemContent );
                Item->setSizeHint( QSize(300,64) );
                Widget = new ListElement;
                Widget->setInfo( User->Uploads->cast_BaseInfo() );
                Widget->setSelected(false);
                ui->ItemContent->setItemWidget( Item, Widget );

                /* This does not work... I think its something with my parser....
                   have to look it up, maybe something with that it never parsed the playlists...
                   Solved it, I think forgot to create a playlist feed :P */
                Item = new QListWidgetItem( ui->ItemContent );
                Item->setSizeHint( QSize(300,64) );
                Widget = new ListElement;
                Widget->setInfo( User->Playlists->cast_BaseInfo() );
                Widget->setSelected(false);
                ui->ItemContent->setItemWidget( Item, Widget );

                break;
            break;
        } case( IT_PlaylistFeed ):
            /// @todo
            break;
        case( IT_ChannelFeed ):
            /// @todo
            break;
        default:
            break;
        }
        mCurContent = Info;
    }
}

void MainWindow::Play_Clicked()
{
    if( mCurrentSel && mCurrentSel->getInfo()->getType() == IT_VideoInfo ) {
        mPlayer->PlayVideo( mCurrentSel->getInfo()->ID );
        mPlayer->show();
    }
}
