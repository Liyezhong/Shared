#!/bin/bash
#########################################################################
#
# This script exports Colorado NetworkLayer stuff for eGits.
#
#########################################################################

################# source folders: ###
PLATFORM_INCLUDES_DIR=../../../Include
PLATFORM_COMPONENTS_DIR=../../../Components
PLATFORM_COMPONENTS=(NetworkComponents Global MasterNetworkClient)
COLORADO_BUILD_DIR=../../../../../Colorado/Master/Components/Main/Build
COLORADO_HOST_REL_BIN_DIR=$COLORADO_BUILD_DIR/bin_rel
COLORADO_HOST_DBG_BIN_DIR=$COLORADO_BUILD_DIR/bin_dbg
COLORADO_TGT_REL_BIN_DIR=$COLORADO_BUILD_DIR/bin_rel_tgt
COLORADO_TGT_DBG_BIN_DIR=$COLORADO_BUILD_DIR/bin_dbg_tgt
COLORADO_GUI_DIR=../../../../../Colorado/Gui/
################################################################

################# folder names which need to be created: ###
FOLDERS_PLATFORM=(Platform Platform/Master Platform/Master/Components)
FOLDERS_TESTFS=(TestFilesystem TestFilesystem/Bin TestFilesystem/Bin/config TestFilesystem/Logfiles)
FOLDERS_COLORADO=(Colorado Colorado/Gui)
LOCAL_COMPONENTS=Platform/Master/Components
LOCAL_MASTER=Platform/Master
LOCAL_TESTFS=TestFilesystem
LOCAL_BIN=TestFilesystem/Bin
LOCAL_BIN_CONFIG=TestFilesystem/Bin/config
LOCAL_COLORADO=Colorado
LOCAL_COLORADO_GUI=Colorado/Gui
################################################################

################# script variables: ###
EXECUTION_STATUS=ok

################# construct target package folder name: ###
targetFolder=$(pwd)/Package4eGits_$(date +%F)_$(date +%H)-$(date +%M)
################# create package folder: ###
echo
echo Package will be stored here: $targetFolder
echo
mkdir $targetFolder
if [ $? != "0" ]; then 
   echo
   echo ***ERROR***: folder $targetFolder cannot be created!
   echo
   exit 1
fi
################# jump to package folder: ###
cd $targetFolder

################# create Platform folder structure: ###
for localFolder in ${FOLDERS_PLATFORM[@]}
do
   echo Creating folder: $localFolder
   mkdir $localFolder
   if [ $? != "0" ]; then 
      echo ***ERROR***: cannot create the $localFolder folder!
      EXECUTION_STATUS=ERROR
   fi
done

################# create TestFilesystem folder structure: ###
for localFolder in ${FOLDERS_TESTFS[@]}
do
   echo Creating folder: $localFolder
   mkdir $localFolder
   if [ $? != "0" ]; then 
      echo ***ERROR***: cannot create the $localFolder folder!
      EXECUTION_STATUS=ERROR
   fi
done

################# create Colorado folder structure: ###
for localFolder in ${FOLDERS_COLORADO[@]}
do
   echo Creating folder: $localFolder
   mkdir $localFolder
   if [ $? != "0" ]; then 
      echo ***ERROR***: cannot create the $localFolder folder!
      EXECUTION_STATUS=ERROR
   fi
done

echo 
echo Copying Colorado/Platform data to output folders...
echo 

################# copy Platform components: ###
for componentFolder in ${PLATFORM_COMPONENTS[@]}
do
   cp -R $PLATFORM_COMPONENTS_DIR/$componentFolder $LOCAL_COMPONENTS
   if [ $? != "0" ]; then 
      echo ***ERROR***: cannot copy Platform components!
      EXECUTION_STATUS=ERROR
   fi
done

################# copy Platform includes: ###
cp -R $PLATFORM_INCLUDES_DIR $LOCAL_MASTER
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Platform includes!
   EXECUTION_STATUS=ERROR
fi

################# copy colorado settings: ###
cp -R $COLORADO_BUILD_DIR/Settings $LOCAL_TESTFS/
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Settings!
   EXECUTION_STATUS=ERROR
fi
cp $LOCAL_TESTFS/Settings/hw_specification.xml $LOCAL_BIN_CONFIG/
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy hw_specification.xml!
   EXECUTION_STATUS=ERROR
fi

################# copy host/target debug/release binaries: ###
cp $COLORADO_HOST_REL_BIN_DIR/colorado $LOCAL_BIN/colorado_host_rel
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Colorado host release binary!
   EXECUTION_STATUS=ERROR
fi
cp $COLORADO_HOST_DBG_BIN_DIR/colorado $LOCAL_BIN/colorado_host_dbg
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Colorado host debug binary!
   EXECUTION_STATUS=ERROR
fi
cp $COLORADO_TGT_REL_BIN_DIR/colorado $LOCAL_BIN/colorado_tgt_rel
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Colorado target release binary!
   EXECUTION_STATUS=ERROR
fi
cp $COLORADO_TGT_DBG_BIN_DIR/colorado $LOCAL_BIN/colorado_tgt_dbg
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy Colorado target debug binary!
   EXECUTION_STATUS=ERROR
fi

####################################################
################# clean up unnecessary stuff --> ###
echo 
echo Removing unnecessary stuff...
echo 
################# remove source folders: ###
find $targetFolder -name "Source" | xargs rm -rf
################# remove test folders: ###
find $targetFolder -name "Test" | xargs rm -rf
################# remove LocalConfig folders: ###
find $targetFolder -name "LocalConfig" | xargs rm -rf
################# remove svn folders: ###
find $targetFolder -name ".svn" | xargs rm -rf
################# remove object folders: ###
find $targetFolder -name "obj_*" | xargs rm -rf
################# remove moc folders: ###
find $targetFolder -name "moc_*" | xargs rm -rf
################# remove makefiles: ###
find $targetFolder -name "Makefile*" | xargs rm -f
################# remove project files: ###
find $targetFolder -name "*.pr*" | xargs rm -f
################# remove old backup files: ###
find $targetFolder -name "*.old" | xargs rm -f

echo 
echo Copying selected GUI sources...
echo 

################# copy selected GUI sources: ###
cp $COLORADO_GUI_DIR/ClientUserExample* $LOCAL_COLORADO_GUI/
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy ClientUserExample!
   EXECUTION_STATUS=ERROR
fi
cp $COLORADO_GUI_DIR/*.pro $LOCAL_COLORADO_GUI/
if [ $? != "0" ]; then 
   echo ***ERROR***: cannot copy GUI project files!
   EXECUTION_STATUS=ERROR
fi

################# report status: ###
if [ $EXECUTION_STATUS == "ok" ]; then 
   echo 
   echo "**************** Export successful ! ****************"
   echo 
   exit 0
fi

echo 
echo "**************** Export failed ! ****************"
echo 
exit 1

