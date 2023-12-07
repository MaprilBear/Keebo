SamacSys ECAD Model
1565631/1211395/2.50/2/4/Crystal or Oscillator

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r240_200"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 2.000) (shapeHeight 2.400))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "NX5032GA16000000MHZLNCD1" (originalName "NX5032GA16000000MHZLNCD1")
		(multiLayer
			(pad (padNum 1) (padStyleRef r240_200) (pt -2.000, 0.000) (rotation 0))
			(pad (padNum 2) (padStyleRef r240_200) (pt 2.000, 0.000) (rotation 0))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt -0.000, 0.000) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.5 1.6) (pt 2.5 1.6) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.5 1.6) (pt 2.5 -1.6) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt 2.5 -1.6) (pt -2.5 -1.6) (width 0.2))
		)
		(layerContents (layerNumRef 28)
			(line (pt -2.5 -1.6) (pt -2.5 1.6) (width 0.2))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4 2.6) (pt 4 2.6) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4 2.6) (pt 4 -2.6) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt 4 -2.6) (pt -4 -2.6) (width 0.1))
		)
		(layerContents (layerNumRef 30)
			(line (pt -4 -2.6) (pt -4 2.6) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.5 1.6) (pt 2.5 1.6) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -2.5 -1.6) (pt 2.5 -1.6) (width 0.1))
		)
	)
	(symbolDef "NX5032GA-16_000M-LN-CD-1" (originalName "NX5032GA-16_000M-LN-CD-1")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 140 mils -15 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 400 mils 0 mils) (rotation 180) (pinLength 100 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 260 mils -15 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 120 mils 100 mils) (pt 120 mils -100 mils) (width 6 mils))
		(line (pt 280 mils 100 mils) (pt 280 mils -100 mils) (width 6 mils))
		(line (pt 160 mils 140 mils) (pt 160 mils -140 mils) (width 6 mils))
		(line (pt 160 mils -140 mils) (pt 240 mils -140 mils) (width 6 mils))
		(line (pt 240 mils -140 mils) (pt 240 mils 140 mils) (width 6 mils))
		(line (pt 240 mils 140 mils) (pt 160 mils 140 mils) (width 6 mils))
		(line (pt 280 mils 0 mils) (pt 300 mils 0 mils) (width 6 mils))
		(line (pt 120 mils 0 mils) (pt 100 mils 0 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 350 mils 250 mils) (justify 24) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "NX5032GA-16.000M-LN-CD-1" (originalName "NX5032GA-16.000M-LN-CD-1") (compHeader (numPins 2) (numParts 1) (refDesPrefix Y)
		)
		(compPin "1" (pinName "X1") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "X2") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "NX5032GA-16_000M-LN-CD-1"))
		(attachedPattern (patternNum 1) (patternName "NX5032GA16000000MHZLNCD1")
			(numPads 2)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
			)
		)
		(attr "Mouser Part Number" "344-NX5032GA16MLNCD1")
		(attr "Mouser Price/Stock" "https://www.mouser.co.uk/ProductDetail/NDK/NX5032GA-16.000M-LN-CD-1?qs=55YtniHzbhDnyNixVDBwYw%3D%3D")
		(attr "Manufacturer_Name" "NDK")
		(attr "Manufacturer_Part_Number" "NX5032GA-16.000M-LN-CD-1")
		(attr "Description" "Crystals CRYSTAL 16MHZ 8PF SMD")
		(attr "Datasheet Link" "https://media.digikey.com/pdf/Data%20Sheets/NDK%20PDFs/NX5032GA.pdf")
		(attr "Height" "1.5 mm")
	)

)
