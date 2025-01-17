#!/bin/bash

############################################################
# Help                                                     #
############################################################
Help()
{
   # Display Help
   echo "Download Files From Webserver script."
   echo
   echo "Syntax: download_files_from_webserver.sh [options]"
   echo "options:"
   echo "help or h        Print this Help.                  "    
   echo "[server]         Destination Server address.                "
   echo "[destination]    File Destination.                 "
   echo
   echo "Command line exemple."
   echo "download_files_from_webserver.sh -help"
   echo "download_files_from_webserver.sh webserver destination "
   echo "download_files_from_webserver.sh 'csb.cidco.ca' '/home/csb/'"
   
}

############################################################
# Command                                                  #
############################################################
Command()
{

rsync -e "ssh" --remove-source-files --partial -z --compress-level=9 -a -H -v --stats csb0001@csb.cidco.ca:/csb/csb0001/data/*.* csb@$server:/csb/csb0001/data/*.* 
   
}


if [ -z "$1" ] || [ "$1" = 'h' ] || [ "$1" = '-h' ] || [ "$1" = 'help' ] || [ "$1" = '-help' ]
then
  Help
  exit
else 
  server=$1
  dest=$2
  
  if [ ! -z "$webadd" ] && [ ! -z "$dest" ]
  then
    Command
    exit
  else 
    Help
    exit
  fi
fi






