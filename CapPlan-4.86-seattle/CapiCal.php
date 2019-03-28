<?php
// copyright by PostWare (S.B.M. Post)
// $Id: CapiCal.php,v 1.1 2016/08/17 18:37:25 darseq Exp $
// This script allows one to connect to a mysql server to access appointment
// data. This allows for employees to look up their own appointments using a
// mobile device for instance. The $login,$passw and default $schedule value
// should be set before using it. The script is called like this:
// CapiCal.pl?schedule=1&employee=1&appsdate=2011-08-03

$login = '<username>';
$passw = '<password>';
$server = 'localhost';
$dbh = mysqli_connect($server, $login, $passw, 'capplan');
if (!$dbh) {print_error("Can't connect to database\n".mysqli_connect_error());}

$employee = isset($_GET["employee"]) ? $_GET["employee"] : "";
$appsdate = isset($_GET["appsdate"]) ? $_GET["appsdate"] : "";
$schedule = isset($_GET["schedule"]) ? $_GET["schedule"] : "";
if ($employee) {
    $employee = mysqli_real_escape_string($dbh, $employee);
} else {print_error("An employee id must be given\n");}
if ($appsdate) {
    $appsdate = mysqli_real_escape_string($dbh, $appsdate);
} else {$appsdate = date("Y-m-d");}
if ($schedule) {
    $schedule = mysqli_real_escape_string($dbh, $schedule);
} else {$schedule = 1;}

$sql = "select date_format(DATE_FROM,'%Y%m%dT%H%i%s') as DATE_FROM,".
        "date_format(DATE_UNTIL,'%Y%m%dT%H%i%s') as DATE_UNTIL,UUID,LASTNAME,".
        "concat(concat(ADDRESS,case when ADDRESS='' then '' else ', ' end),".
        "concat(POSTCODE,case when POSTCODE='' then '' else ', ' end),CITY) ".
        "as LOCATION,TELEPHONE,TELEPHONE2,INFOTEXT,CANCELLED from (apps right ".
        "join entries on apps.APP_ID=entries.APP_ID) left join personal on ".
        "apps.PERSONAL_ID=personal.PERSONAL_ID where (entries.EMPLOYEE_ID=".
        "$employee) and DATE_FROM<DATE_ADD('$appsdate',INTERVAL 1 MONTH) ".
        "and DATE_UNTIL>DATE_ADD('$appsdate',INTERVAL -1 MONTH) order by ".
        "EMPLOYEE_ID,DATE_FROM,DATE_UNTIL,CANCELLED desc";

if ($sth = mysqli_query($dbh, $sql)) {
    $calendar_id = "CapPlan-$schedule-$employee";
    header('Content-type: application/calendar');
    header('Content-disposition: inline; filename="calendar.ics"');
    header('Pragma: no-cache');
    header('Expires: 0');
    echo "BEGIN:VCALENDAR\r\n";
    echo "VERSION:2.0\r\n";
    echo "PRODID:-//PostWare//CapPlan iCal Export//EN\r\n";
    echo "METHOD:PUBLISH\r\n";
    echo "CALSCALE:GREGORIAN\r\n";
    echo "X-WR-RELCALID:$calendar_id\r\n";
    //echo "X-WR-CALNAME:$calendar_id\r\n";
    while ($row = mysqli_fetch_assoc($sth)) {
        $cancelled = $row["CANCELLED"];
        # if entry is blocked, cancelled is undefined
        if ($cancelled === "0") { //  && $cancelled == '0') {
            $uid = $row["UUID"];
            $sequence = time();
            $date_stamp = date("Ymd\THis");
            $date_from = $row["DATE_FROM"];
            $date_until = $row["DATE_UNTIL"];
            $lastname = EscapeFoldStr($row["LASTNAME"]);
            $location = EscapeFoldStr($row["LOCATION"]);
            $telephone = $row["TELEPHONE"];
            //$telephone2 = $row["TELEPHONE2"];
            $infotext = EscapeFoldStr($row["INFOTEXT"]);

            echo "BEGIN:VEVENT\r\n";
            echo "UID:$uid\r\n";
            echo "SEQUENCE:$sequence\r\n";
            echo "ORGANIZER;CN=CapPlan:mailto:capplan@companyname.nl\r\n";
            echo "DTSTAMP;TZID=Europe/Amsterdam:$date_stamp\r\n";
            echo "DTSTART;TZID=Europe/Amsterdam:$date_from\r\n";
            echo "DTEND;TZID=Europe/Amsterdam:$date_until\r\n";
            echo "SUMMARY:$lastname\r\n";
            echo "LOCATION:$location\r\n";
            echo "URL:tel://$telephone\r\n";
            echo "DESCRIPTION:$infotext\r\n";
            echo "END:VEVENT\r\n";
        }
    }
    echo "END:VCALENDAR\r\n";
    mysqli_free_result($sth);
}
mysqli_close($dbh);

function EscapeFoldStr($text) {
    $text = str_replace(
        array("\\", ";", ",", "\r", "\n"),
        array("\\\\", "\\;", "\\,", "", "\\n"),
	$text);

    #Fold text
    $str = ""; $chars = strlen($text);
    for($line=0;$line < $chars/50;$line++) {
        if ($line) {$str .= "\r\n ";}
        $str .= substr($text, $line*50, 50);
    }
    return $str;
}

function print_error($message) {
    header('Content-type: text/plain');
    header('Status: 400 Bad Request');
    if (!$message) {$message = "Unknown error";}
    echo "$message";
    exit(1);
}
