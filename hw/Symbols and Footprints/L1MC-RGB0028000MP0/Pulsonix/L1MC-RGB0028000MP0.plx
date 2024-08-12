PULSONIX_LIBRARY_ASCII "SamacSys ECAD Model"
//16494331/1234350/2.50/6/3/LED

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r105_56"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.560) (shapeHeight 1.050))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Normal"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 1.27)
			(strokeWidth 0.127)
		)
	)
	(patternDef "L1MCRGB0028000MP0" (originalName "L1MCRGB0028000MP0")
		(multiLayer
			(pad (padNum 1) (padStyleRef r105_56) (pt -1.425, -0.850) (rotation 90))
			(pad (padNum 2) (padStyleRef r105_56) (pt 1.425, -0.850) (rotation 90))
			(pad (padNum 3) (padStyleRef r105_56) (pt -1.425, 0.000) (rotation 90))
			(pad (padNum 4) (padStyleRef r105_56) (pt 1.425, 0.000) (rotation 90))
			(pad (padNum 5) (padStyleRef r105_56) (pt -1.425, 0.850) (rotation 90))
			(pad (padNum 6) (padStyleRef r105_56) (pt 1.425, 0.850) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0.000, 0.000) (textStyleRef "Normal") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.6 1.4) (pt 1.6 1.4) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.6 1.4) (pt 1.6 -1.4) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.6 -1.4) (pt -1.6 -1.4) (width 0.025))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.6 -1.4) (pt -1.6 1.4) (width 0.025))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -2.95 2.4) (pt 2.95 2.4) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 2.95 2.4) (pt 2.95 -2.4) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt 2.95 -2.4) (pt -2.95 -2.4) (width 0.1))
		)
		(layerContents (layerNumRef Courtyard_Top)
			(line (pt -2.95 -2.4) (pt -2.95 2.4) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.6 1.4) (pt 0.6 1.4) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -0.6 -1.4) (pt 0.6 -1.4) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.4 -0.85) (pt -2.4 -0.85) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -2.35, -0.85) (radius 0.05) (startAngle 180.0) (sweepAngle 180.0) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.3 -0.85) (pt -2.3 -0.85) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(arc (pt -2.35, -0.85) (radius 0.05) (startAngle .0) (sweepAngle 180.0) (width 0.1))
		)
	)
	(symbolDef "L1MC-RGB0028000MP0" (originalName "L1MC-RGB0028000MP0")

		(pin (pinNum 1) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 2) (pt 1400 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1170 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 3) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 4) (pt 1400 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1170 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(pin (pinNum 5) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Normal"))
		))
		(pin (pinNum 6) (pt 1400 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName true)) (pinName (text (pt 1170 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Normal"))
		))
		(line (pt 200 mils 100 mils) (pt 1200 mils 100 mils) (width 6 mils))
		(line (pt 1200 mils 100 mils) (pt 1200 mils -300 mils) (width 6 mils))
		(line (pt 1200 mils -300 mils) (pt 200 mils -300 mils) (width 6 mils))
		(line (pt 200 mils -300 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 1250 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))
		(attr "Type" "Type" (pt 1250 mils 200 mils) (justify Left) (isVisible True) (textStyleRef "Normal"))

	)
	(compDef "L1MC-RGB0028000MP0" (originalName "L1MC-RGB0028000MP0") (compHeader (numPins 6) (numParts 1) (refDesPrefix LED)
		)
		(compPin "1" (pinName "K_GREEN") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "2" (pinName "A_GREEN") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "3" (pinName "K_RED") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "4" (pinName "A_RED") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "5" (pinName "K_BLUE") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Unknown))
		(compPin "6" (pinName "A_BLUE") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Unknown))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "L1MC-RGB0028000MP0"))
		(attachedPattern (patternNum 1) (patternName "L1MCRGB0028000MP0")
			(numPads 6)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
			)
		)
		(attr "Mouser Part Number" "997-L1MCRGB00280MP")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/Lumileds/L1MC-RGB0028000MP0?qs=Znm5pLBrcAJSGF05jr80lQ%3D%3D")
		(attr "Manufacturer_Name" "Lumileds")
		(attr "Manufacturer_Part_Number" "L1MC-RGB0028000MP0")
		(attr "Description" "LED Lighting Color LUXEON 3528 RGB Red, Green, Blue (RGB) 622nm Red, 525nm Green, 470nm Blue 6-PLCC")
		(attr "<Hyperlink>" "https://lumileds.com/wp-content/uploads/2022/03/DS388-luxeon-3528-rgb-datasheet.pdf")
		(attr "<Component Height>" "1.85")
		(attr "<STEP Filename>" "L1MC-RGB0028000MP0.stp")
		(attr "<STEP Offsets>" "X=0;Y=0;Z=0")
		(attr "<STEP Rotation>" "X=0;Y=0;Z=0")
	)

)