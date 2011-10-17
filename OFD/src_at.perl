#!/usr/bin/perl -w
use strict;
use Device::Gsm;

#Connexion au port du modem en precisant le code PIN et en activant la connexion
my $gsm = new Device::Gsm(port=>'/dev/ttyACM0', pin=>'0000', assume_registered=>1);
if($gsm->connect()) {print "Modem connexion OK !\n";}
else {print "Erreur code PIN ou modem !\n"; exit 1;}

#Connecte au reseau
if ($gsm->register()){print "register OK !\n";
}else{print "#Erreur register !\n"; exit 1;}

#Informations SIM/ et modem
print "\nIMEI : ", $gsm->imei();
print "\nProduit : ", $gsm->manufacturer();
print "\nModel : ", $gsm->model();
print "\nVersion : ", $gsm->software_version();
print "\nDate(sys) : ", $gsm->datetime();
print "\nOperateur : ", $gsm->network();
print "\nSignal : ", $gsm->signal_quality(), "/-51\n";

#Afficher les SMS (carte SIM=SM/telephone=ME)
print "\nSMS : ",$gsm->storage('SM'), $gsm->messages();
#Envoi de SMS : $gsm->send_sms(recipient=>'0612345678', content=>'Hello world!');
#Suppression de SMS : $gsm->delete_sms(3);

#Tester si une commande existe
print "\nVERIF SI LA CMD EST GEREE [AT+CSCA] : ", $gsm->test_command('CSCA');

#Envoi direct de commande AT
print "\nAT CMD :", $gsm->atsend('AT+CSCA' . Device::Modem::CR), $gsm->_answer(), "\n";
