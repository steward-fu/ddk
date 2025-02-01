        parmD   lpDestDev               ;--> to destination bitmap descriptor
        parmW   DestxOrg                ;Destination origin - x coordinate
        parmW   DestyOrg                ;Destination origin - y coordinate
        parmD   lpSrcDev                ;--> to source bitmap descriptor
        parmW   SrcxOrg                 ;Source origin - x coordinate
        parmW   SrcyOrg                 ;Source origin - y coordinate
        parmW   xExt                    ;x extent of the BLT
        parmW   yExt                    ;y extent of the BLT
        parmD   Rop                     ;Raster operation descriptor
        parmD   lpPBrush                ;--> to a physical brush (pattern)
        parmD   lpDrawMode              ;--> to a drawmode
