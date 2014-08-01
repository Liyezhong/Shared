R.Wobst, @(#) Aug 31 2011, 10:46:07

ImportExport - Stand
====================

STRUKTUR IN SVN:

  Die Software befindet sich in SVN unter

  svn_trunk/Platform/Master/Components/ImportExport

  und besteht derzeit aus den Subdirectories

  AbstractFile		# vgl. Kommentar zu ReadArchive unten
  AES			# AES-Verschlüsselung und -Entschlüsselung
  CompressEncrypt	# Komprimieren, ggf. Chiffrieren, Split in Chunks
  CryptoService		# Key handling, HMACs+Hash berechnen, Chiffrierung
  DecryptUncompress	# Umkehrung zu CompressEncrypt
  General		# allg. Hilfsklassen und Konstanten
  HMAC			# HMAC-Berechnung (nicht in Qt enthalten)
  ReadArchive		# Einlesen, Checken und Auspacken eines Archivs
  WriteArchive		# Erzeugen eines Archivs
  Build			# Erzeugen einer Bibliothek libImportExport.a

  Jedes Directory enthält die Subdirectories Include, Source und Test.
  In jedem Test-Directory befindet sich ein Projektfile mit Namen
  utTest*.pro mit Unittests für diese Einheit. Es wird vorerst nur im
  Debugmode gebildet. Vom Buildbot aus werden nur die komplexen
  Unittests in ReadArchive und WriteArchive ausgeführt; Fehler in
  Submodulen wirken sich sofort auf diese beiden Testgruppen aus.

  Zusätzlich finden sich im Subdirectory Build noch die üblichen
  *pro/*pri-Files, um Bibliotheken zu erzeugen. Diese Bibliotheken
  werden von Unittests in ReadArchive und WriteArchive genutzt.

  Die *.pri-Files in ReadArchive/Test und WriteArchive/Test sind
  modifizierte Kopien der pri-Files von
  svn_trunk/Platform/Master/Test/Platform.pri, weil die Variable
  COMPONENTDIR bei der ImportExport-Komponente auf eine Ebene
  höher verweisen muss.

  Außerdem gibt es noch in ReadArchive, WriteArchive und
  DecryptUncompress Directorys Test/testresults mit Binärfiles für 
  Unittests. In ReadArchive/Test/testresults findet sich ein
  Unterprojekt GenerateTestArchives.pro für die Erzeugung von
  Testarchiven.

  Eine Reihe von Pythonskripten dient der Erzeugung weiterer Testdaten
  bzw. der Analyse erzeugter Archive und ist im Quellcode dokumentiert.


API
===

Als API dienen momentan nur die beiden Funktionsrufe

void WriteArchive(QByteArray archive_name,
	   QList<QByteArray> filelst,
	   bool encrypt = false,
	   qint16 key_version=0);

für den Export und

void ReadArchive(QByteArray archive_name,
	   AbstractFile* fd,
	   QByteArray purpose,
           QByteArray keydata = QByteArray());

für Import/Viewer/Checker. Details finden sich in der Doxygen-Dokumentation.

Bei Fehlern wird die Ausnahme ImexException geworfen und muss vom Caller
angefangen werden. Über deren Methode getMsg() kann die Fehlernachricht
ermittelt werden (vgl. Doxygen-Dokumentation).

Der Parameter fd bei ReadArchive ist eine abstrakte Fileschnittstelle,
die beim Import das Schreiben auf physische Files realisiert (mit den
Methoden open(), write() und close()). Beim Viewer wird statt dessen ein
QHash erzeugt mit Filenamen als Keys und Fileinhalten als Values. Beim
Checker werden nur HMACs überprüft und die Filenamen in einem QSet
gesammelt.

Beide Funktionen sind in der Bibliothek libImportExport.a implementiert.


ARCHIVFORMAT
============

Das Archivformat ist im SDS-Manuskript genau beschrieben und kritisch.
Es sollte nicht verändert werden, obwohl ein Feld "Formatversion" im
Header für alle Fälle enthalten ist.

Auf Grundlage dieser Informationen ist es nicht schwer, ein Archiv auch
mit anderen Programmen auszupacken, vorausgesetzt, man kennt die
Schlüssel.

Beim Entpacken wird jeweils nur ein Schlüssel benötigt: Beim Import ein
Schlüssel C, der aus dem "Generalschlüssel" B und einem festen String S0
berechnet wird und absichert, dass nur von gleichen Stainern (bzw. von
Leica) generierte Archive importiert werden können. Der
"Generalschlüssel" B wird vom Viewer zum Überprüfen der Integrität
genutzt, wobei Gerätenummer und Folgenummer bekannt sein müssen (die aus
dem Header extrahiert werden können). Den "Geheimschlüssel" A
schließlich darf nur Leica kennen; mit ihm lassen sich Archive
unabhängig von evtl. kompromittierter Viewer-Software prüfen.

Das Format und die Art der HMAC-Berechnung sichern Integrität,
Reihenfolge und Vollständigkeit von Header und Archiveinträgen ab.


ART DER NUTZUNG
===============

Export:

Für den Export soll WriteArchive in einer gesonderten Applikation
gestartet werden, mit niedriger CPU-Priorität (nice-Kommando) und
I/O-Priorität (ionice-Kommando). Archivname (und damit auch die
Seriennummer), Fileliste und Encryption Flag werden dabei als
Kommandozeilenargumente übergeben. Die Fehlermeldungen und ggf.
Warnungen können dabei über eine Pipeline vom Caller empfangen werden,
wenn der Prozess mit QProcess gestartet wird.

Sinn des Starts einer gesonderten Applikation ist es, zeitkritisch
Prozesse nicht zu behindern. Das ist beim Export über Remote Care
wichtig.

Entweder sind alle Archiveinträge chiffriert (Logfiles), oder alle im
Klartext (genauer, nur komprimiert). Für den Viewer und Remote Care
werden also i.a. zwei Archive erzeugt (eines davon chiffriert).

Es kann sinnvoll sein, beim Export aus irgendeinem Grund nicht lesbare
oder nicht existente Files mit einer Warnung zu überspringen, damit z.B.
Remote Care oder Viewer noch Informationen zum Auswerten bekommen.

Ebenso könnte es noch einen zusätzlichen Parameter "backup" geben: Das
Archiv wird nur zum Backup erzeugt. Der Index in der Hash Chain braucht
dabei nicht erhöht zu werden.

Der Import kann ausschließlich vom Nutzer angestoßen werden uns sollte
wegen der entsprechenden Überprüfungen der Inhalte innerhalb der
Mastersoftware laufen.

Denkbar wäre eine Importoption (Parameter 'purpose') "Dummy", die HMACs
nicht prüft. Allerdings braucht man das Keymanagement zum Auswerten
chiffrierter Archive unbedingt.

Alles Logging muss vom Caller aus geschehen.


TODO
====

- Die Namen der beiden Files, in denen die Schlüssel mit Counter (auf
  NAND-Flash) und der Counter auf der SD-Karte gespeichert werden, sind
  vorerst noch in General::Constants fest eingebrannt. Sie sollten als
  Parameter übergeben werden. Nach jetzigem Stand muss das über
  ReadArchive und WriteArchive geschehen, die es an CryptoService
  weitergeben.

  Sollte das Gerät mit Festplatte statt SD-Karte gebaut werden, entfällt
  die gesamte Logik für den auf SD-Karte gespeicherten
  Hashchain-Counter. Der Konstruktor von CryptoService (ab Zeile 105)
  sollte dann stark vereinfacht werden.

- Das Caller Interface existiert noch nicht, das Read/WriteArchive ruft
  und Exceptions auswertet (und beim Export die Error messages an den
  Caller sendet).

- Der Caller bei der Exportfunktion sollte die Liste der Filenamen
  aus einem XML-Archiv entnehmen. Shell-Metazeichen könnten hier
  sinnvoll sein, um z.B. Gruppen von Logfiles mit dynamisch generierten
  Namen einzupacken (QRegExp::Wildcard mit Shell-Option,
  QDir::listEntry() mit Filter).

- Die Archivnamen für Backup-Archiv und Logfile-Archiv müssen noch
  festgelegt werden, genauer, der 'info' Teil in

  Colorado_info_serialnumber_timestamp

  (vgl. SRSImportExport.doc 3.9.5 und SDSImportExport.doc 4.1.1).
