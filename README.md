![alt text](https://github.com/sbmpost/CapPlan/blob/master/CapPlan.png "CapPlan")

# CapPlan installation instructions for OSX & Additional notes

Open the "Terminal" application and run the commands as described in the steps below.

CapPlan Prerequisites
------------------------

1. run: /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
2. run: brew install libpng freetype wget
3. run: wget -O $HOME/Downloads/CapPlan-4.86-OSX.zip https://github.com/sbmpost/CapPlan/blob/master/CapPlan-4.86-OSX.zip?raw=true
4. run: unzip $HOME/Downloads/CapPlan-4.86-OSX.zip -d $HOME/Downloads
5. run: cp $HOME/Downloads/CapPlan-4.86-OSX/mail_doc.sh /usr/local/bin
6. run: cp -r $HOME/Downloads/CapPlan-4.86-OSX/CapPlan.app $HOME/Applications

Install UnixODBC
------------------------

7. run: brew install unixodbc
8. run: open $HOME/Downloads/CapPlan-4.86-OSX/iodbc.dmg (and double click the pkg file)
9. run: open $HOME/Downloads/CapPlan-4.86-OSX/myodbc.dmg (and double click the pkg file)
10. run: cp $HOME/Downloads/CapPlan-4.86-OSX/odbc* /usr/local/etc
11. make symbolic links (this will ask for the administrator password):
  - run: sudo mv /Library/ODBC/odbc.ini /Library/ODBC/odbc.orig.ini
  - run: sudo mv /Library/ODBC/odbcinst.ini /Library/ODBC/odbcinst.orig.ini
  - run: sudo ln -s /usr/local/etc/odbc.ini /Library/ODBC/odbc.ini
  - run: sudo ln -s /usr/local/etc/odbcinst.ini /Library/ODBC/odbcinst.ini

12. install MySQL (not needed if MySQL is already installed on another server)
  - run: brew install mysql
  - run: nano /usr/local/etc/my.cnf
  - edit: innodb_lock_wait_timeout = 1 (add this line)
  - save: use CTRL-X and answer with y + ENTER
  - run: brew services start mysql
  - optional: refer to MySQL documentation for information on making database backups

13. update odbc.ini settings:
  - run: nano /Library/ODBC/odbc.ini
  - edit: Password=MySQL root password (to setup a non-root user, see note A below)
  - edit: Server=MySQL server ip (for example 10.0.0.221)
  - save: use CTRL-X and answer with y + ENTER

14. check ODBC installation with:
  - run: odbcinst -j      (file locations)
  - run: odbcinst -q -s   (data sources)
  - run: odbcinst -q -d   (drivers)

Install CapPlan
------------------------

15. run: $HOME/Applications/CapPlan.app/Contents/MacOS/install.sh
16. run: chmod +x $HOME/Applications/CapPlan.app/Contents/Resources/wine-prefix/drive_c/users/$USER/CapPlan/*.sh
17. run: open $HOME/Applications/CapPlan.app
18. in CapPlan: check document paths via Tools | Options | Folders menu
19. in CapPlan: add an order.pdf document via Archive | Documents | New
20. in CapPlan: open the added order.pdf document via Archive | Documents | Open (see also note B below)
21. allow access to "System Events" via System Preferences | Security & Privacy | Privacy | Accessibility
22. CapPlan installation is now done, below are some additional notes which might be useful

Additional (admin) Notes
------------------------

A) After a successful CapPlan setup, a non-root user can be created:
  - create the non-root user (refer to the MySQL documentation to see how). 
  - run: nano /Library/ODBC/odbc.ini
  - edit: User=MySQL non-root user
  - edit: Password=MySQL user password
  - save: use CTRL-X and answer with y + ENTER
  - note that the default schedule that CapPlan creates is owned by root and invisible to other users. The
    root user can unhide the default schedule via: Archive | Schedules | Details. non-root users can also
    create schedules which by default are only visible to them and the root user.

B) CapPlan integrates with Apple's Preview application. To integrate with PDF Studio 9, do the following:
  - run: cd $HOME/Applications/CapPlan.app/Contents/Resources/wine-prefix/drive_c/users/$USER/CapPlan
  - run: cp open_doc.sh open_doc_preview.sh
  - run: cp open_doc_pdf_studio.sh open_doc.sh  

  If another PDF integration is desired you will have to modify the "open_doc.sh" file and create a script
  that goes with it. For inspiration on how to do the latter, have a look at the *.scpt files which can be
  opened with "Script Editor". Also useful to know is that when CapPlan opens a pdf file, open_doc.sh gets
  called with two parameters: $1=the pdf file to open, $2=a temporary file. The existence of the temporary
  file is periodically checked by a program called "spawn_open.exe" and CapPlan will consider the pdf file
  closed once the temporary file is removed. In turn open_doc.sh calls osascript to monitor the actual pdf
  application. So the flow is as follows:
   
  CapPlan.exe -> spawn_open.exe -> open_doc.sh -> osascript (open_wait.scpt/wait_quit.scpt/wait_save.scpt)

C) Multiple CapPlan clients can connect to the same database server. CapPlan is multiuser aware and will detect
   simultaneous changes to data and warn the user if clashes occur. As for documents, 3rd party synchronisation
   software can be used to sync them between the clients, which allows for faster editing. This could happen every
   15 minutes for example. If document modification timestamps are correctly preserved during the sync, CapPlan
   will check for consistency. Furthermore CapPlan will prevent simultaneous modifications to the same document.

D) If you want to use the Dutch version of CapPlan you can do this:
  - run: nano $HOME/Applications/CapPlan.app/Contents/MacOS/capplan.sh
  - edit: export LC_ALL="nl_NL.UTF-8"
  - edit: APP="$(winepath "c:\\\\users\\\\$USER\\\\CapPlan\\\\CapPlan_NLD.exe")"
  - save: use CTRL-X and answer with y + ENTER

E) The CapPlan source and binaries are copyright by PostWare (sbmpost). This includes the files in these archives:
  - CapPlan-4.86-OSX.zip
  - CapPlan-4.86-WIN.zip
  - CapPlan-4.86-seattle
