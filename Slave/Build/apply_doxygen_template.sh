#!/bin/sh
find .. -iname "doxygen.cfg" \
| while read FILENAME; do 
    TITLE=${FILENAME%/Document*}
    TITLE=${TITLE#*Components/}
    TITLE=$(echo "$TITLE" | sed 's/\// - /')
    echo "  $TITLE: $FILENAME" 
    sed "s/PROJECT_NAME_TEMPLATE/$TITLE/" doxygen_template.cfg >"$FILENAME"
    if [ "$TITLE" = "FunctionModules - Steppermotor" ]; then 
        sed -i  's#../Include#& ../../../../../Common/Components/FunctionModules#' "$FILENAME"
    fi
done
echo "Done."

