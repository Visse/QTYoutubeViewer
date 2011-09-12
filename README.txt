**** QT YoutubeViewer ****
Lisence: GPL v3 [ http://www.gnu.org/licenses/gpl-3.0-standalone.html#terms ]

This program is currently in an early Alpha stage, its unstable, atleast the play function is :(
But atleast it I am working on making it more stable :)


*** Trouble Shooting ***
* If every element is black, or gray make sure that the file 'Settings.cfg'
    is in the folder there you are running the program.
* If you can't play any video, there is just a new window open with some controlls,
    Don't worry, the play function is not near finiched, I have only got it working on
    my linux machine ATM and that just partilally( no seak... ).

* If you think you have found a bug, you think wrong ( I does not make any bugs :P )
    just kidding, mail me a description of the bug and the log ( 'YoutubeViewer.log' )
    my mail is in the 'contact me' section :), hope fully there is a bugfix out :),
    or if you found a solution to the bug to( that would be awsome! ) you saved me some work :),
    mail me the solution so other people that have that bug also can enjoy the fix :)


**** Usage ****
There is a input box on top with a button besides it, there you can write a url or a search term.
Valids url are:
XX.. = ID or username.

Video: http://www.youtube.com/watch?v=XXXXXXXXXXX
Playlist: http://www.youtube.com/playlist?list=PLXXXXXXXXXXXXXXXX
User: http://www.youtube.com/user/XXXXX

REMEMBER: VideoID's IS casesensitive, usernames and playlistsID's are NOT.


The urls can also be 'feed urls', these is not so mush for the avarage user, but here there are:

Video: http://gdata.youtube.com/feeds/api/videos/XXXXXXXXXXX?v=2
Playlist: http://gdata.youtube.com/feeds/api/playlists/XXXXXXXXXXXXXXXX?v=2
User: http://gdata.youtube.com/feeds/api/users/XXXXX?v=2

if you wan't to search for a video/playlist/chanel you can use the following keywords:
Video search: 'video:'
Playlist search: 'playlist:'
Chanel search: 'chanel:'

if you does not write a keyword it will asume that its a video search you wan't


ATM its only the video search that is working corecly.

If you now the id of the video/playlist you can write:
Video: 'videoID:'
Playlist: 'playlistID'

and if you now the user name you can write 'user:'



*** WARNING ***
The videoplayer is VERY unstable!, I been albe to track it down,
but not solve it, I think it some kind of deadlock in GStreamer. On
Windows you has to have codex for .flv and .mp4 installed to be able
to play videos, it's the formats that youtube provides, on linux there
should allready be installed.

Thanks to youtube-dl ( http://rg3.github.com/youtube-dl/documentation.html ),
looking thrue the source has been VERY helpfull.


If you find any bugs please report them :), otherwise they won't be solved


And, if you wonder why I'm using sockets to stream video instead of QT's
networkmanager, it's because I wanted more controll, and ofcourse because I
realy like to learn new techniques.

This is act


*** TODO ***
* Add download function - should not be so hard, I allready stream direcly from youtube.
* Add support for windows - well, it allready works on windows,
                            but there is some trouble with the play function :)
* Add support for playlist search and channel search :)
* Fix colors - there seems to be different colors on different system, I think QT takes it
                color scheme from the current theme, but for example if you have a white background,
                wich is the default on most system, you won't be able to see much, I made a quick fix,
                look in 'main.cpp' there I manually set up backgrounds :/
* Add support for comments


*** Contact me ***
You can contact me by mail[ visse44@gmail.com ] and I will try to answer it so fast as I can :)
