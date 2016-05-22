#!/bin/sh

case $1 in
   0)
      case $2 in
         0)
            make
            ;;
         1)
            make
            ./pro_mac
            ;;
         *)
            echo “Option error!”
            ;;
      esac
      ;;
   1)
      case $2 in
         0)
            make clean
            make
            ;;
         1)
            make clean
            make
            ./pro_mac
            ;;
         *)
            echo “Option error!”
            ;;
      esac
      ;;
   *)
      echo “Option error!”
      ;;
esac