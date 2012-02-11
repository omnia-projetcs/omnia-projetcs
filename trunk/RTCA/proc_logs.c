//------------------------------------------------------------------------------
// Projet RTCA          : READ TO CATCH ALL
// Auteur               : Nicolas Hanteville
// Site                 : http://code.google.com/p/omnia-projetcs/
// Licence              : GPL V3
//------------------------------------------------------------------------------
#include "resource.h"
//------------------------------------------------------------------------------
BOOL DefaultEventIdtoDscr(unsigned int eventId, char *result, unsigned short max_size)
{
    switch(eventId)
    {
      case 7 : snprintf(result,max_size,"%05d (Successful auto update retrieval of third-party root list sequence number from :)",eventId);break;
      case 35 : snprintf(result,max_size,"%05d (Time service synchronised)",eventId);break;
      case 36 : snprintf(result,max_size,"%05d (Time service not synchronised)",eventId);break;
      case 115 : snprintf(result,max_size,"%05d (System Restore monitoring enabled on all drives)",eventId);break;
      case 116: snprintf(result,max_size,"%05d (System Restore monitoring disable on all drives)",eventId);break;
      case 512 : snprintf(result,max_size,"%05d (Windows NT is starting up)",eventId);return TRUE;
      case 513 : snprintf(result,max_size,"%05d (Windows is shutting down)",eventId);return TRUE;
      case 514 : snprintf(result,max_size,"%05d (An authentication package has been loaded by the Local Security Authority)",eventId);break;
      case 515 : snprintf(result,max_size,"%05d (A trusted logon process has registered with the Local Security Authority)",eventId);break;
      case 516 : snprintf(result,max_size,"%05d (Internal resources allocated for the queuing of audit messages have been exhausted, leading to the loss of some audits)",eventId);break;
      case 517 : snprintf(result,max_size,"%05d (The audit log was cleared)",eventId);return TRUE;
      case 518 : snprintf(result,max_size,"%05d (A notification package has been loaded by the Security Account Manager)",eventId);break;
      case 519 : snprintf(result,max_size,"%05d (A process is using an invalid local procedure call (LPC) port)",eventId);break;
      case 520 : snprintf(result,max_size,"%05d (The system time was changed)",eventId);return TRUE;
      case 528 : snprintf(result,max_size,"%05d (Successful Logon)",eventId);return TRUE;
      case 529 : snprintf(result,max_size,"%05d (Logon Failure - Unknown user name or bad password)",eventId);return TRUE;
      case 530 : snprintf(result,max_size,"%05d (Logon Failure - Account logon time restriction violation)",eventId);return TRUE;
      case 531 : snprintf(result,max_size,"%05d (Logon Failure - Account currently disabled)",eventId);return TRUE;
      case 532 : snprintf(result,max_size,"%05d (Logon Failure - The specified user account has expired)",eventId);return TRUE;
      case 533 : snprintf(result,max_size,"%05d (Logon Failure - User not allowed to logon at this computer)",eventId);return TRUE;
      case 534 : snprintf(result,max_size,"%05d (Logon Failure - The user has not been granted the requested logon type at this machine)",eventId);return TRUE;
      case 535 : snprintf(result,max_size,"%05d (Logon Failure - The specified account's password has expired)",eventId);return TRUE;
      case 536 : snprintf(result,max_size,"%05d (Logon Failure - The NetLogon component is not active)",eventId);return TRUE;
      case 537 : snprintf(result,max_size,"%05d (Logon failure - The logon attempt failed for other reasons)",eventId);return TRUE;
      case 538 : snprintf(result,max_size,"%05d (User Logoff)",eventId);return TRUE;
      case 539 : snprintf(result,max_size,"%05d (Logon Failure - Account locked out)",eventId);break;
      case 540 : snprintf(result,max_size,"%05d (Successful Network Logon)",eventId);return TRUE;
      case 551 : snprintf(result,max_size,"%05d (User initiated logoff)",eventId);break;
      case 552 : snprintf(result,max_size,"%05d (Logon attempt using explicit credentials)",eventId);break;
      case 560 : snprintf(result,max_size,"%05d (Object Open)",eventId);break;
      case 561 : snprintf(result,max_size,"%05d (Handle Allocated)",eventId);break;
      case 562 : snprintf(result,max_size,"%05d (Handle Closed)",eventId);break;
      case 563 : snprintf(result,max_size,"%05d (Object Open for Delete)",eventId);break;
      case 564 : snprintf(result,max_size,"%05d (Object Deleted)",eventId);break;
      case 565 : snprintf(result,max_size,"%05d (Object Open (Active Directory))",eventId);break;
      case 566 : snprintf(result,max_size,"%05d (Object Operation (W3 Active Directory))",eventId);break;
      case 567 : snprintf(result,max_size,"%05d (Object Access Attempt)",eventId);break;
      case 576 : snprintf(result,max_size,"%05d (Special privileges assigned to new logon)",eventId);return TRUE;
      case 577 : snprintf(result,max_size,"%05d (Privileged Service Called)",eventId);break;
      case 578 : snprintf(result,max_size,"%05d (Privileged object operation)",eventId);break;
      case 592 : snprintf(result,max_size,"%05d (A new process has been created)",eventId);break;
      case 593 : snprintf(result,max_size,"%05d (A process has exited)",eventId);break;
      case 594 : snprintf(result,max_size,"%05d (A handle to an object has been duplicated)",eventId);break;
      case 595 : snprintf(result,max_size,"%05d (Indirect access to an object has been obtained)",eventId);break;
      case 600 : snprintf(result,max_size,"%05d (A process was assigned a primary token)",eventId);break;
      case 601 : snprintf(result,max_size,"%05d (Attempt to install service)",eventId);break;
      case 602 : snprintf(result,max_size,"%05d (Scheduled Task created)",eventId);break;
      case 608 : snprintf(result,max_size,"%05d (User Right Assigned)",eventId);break;
      case 609 : snprintf(result,max_size,"%05d (User Right Removed)",eventId);break;
      case 610 : snprintf(result,max_size,"%05d (New Trusted Domain)",eventId);break;
      case 611 : snprintf(result,max_size,"%05d (Removing Trusted Domain)",eventId);break;
      case 612 : snprintf(result,max_size,"%05d (Audit Policy Change)",eventId);break;
      case 613 : snprintf(result,max_size,"%05d (IPSec policy agent started)",eventId);break;
      case 614 : snprintf(result,max_size,"%05d (IPSec policy agent disabled)",eventId);break;
      case 615 : snprintf(result,max_size,"%05d (IPSEC PolicyAgent Service)",eventId);break;
      case 616 : snprintf(result,max_size,"%05d (IPSec policy agent encountered a potentially serious failure)",eventId);break;
      case 617 : snprintf(result,max_size,"%05d (Kerberos Policy Changed)",eventId);break;
      case 618 : snprintf(result,max_size,"%05d (Encrypted Data Recovery Policy Changed)",eventId);break;
      case 619 : snprintf(result,max_size,"%05d (Quality of Service Policy Changed)",eventId);break;
      case 620 : snprintf(result,max_size,"%05d (Trusted Domain Information Modified)",eventId);break;
      case 621 : snprintf(result,max_size,"%05d (System Security Access Granted)",eventId);break;
      case 622 : snprintf(result,max_size,"%05d (System Security Access Removed)",eventId);break;
      case 623 : snprintf(result,max_size,"%05d (Per User Audit Policy was refreshed)",eventId);break;
      case 624 : snprintf(result,max_size,"%05d (User Account Created)",eventId);return TRUE;
      case 625 : snprintf(result,max_size,"%05d (User Account Type Changed)",eventId);return TRUE;
      case 626 : snprintf(result,max_size,"%05d (User Account Enabled)",eventId);return TRUE;
      case 627 : snprintf(result,max_size,"%05d (Change Password Attempt)",eventId);return TRUE;
      case 628 : snprintf(result,max_size,"%05d (User Account password set)",eventId);return TRUE;
      case 629 : snprintf(result,max_size,"%05d (User Account Disabled)",eventId);return TRUE;
      case 630 : snprintf(result,max_size,"%05d (User Account Deleted)",eventId);return TRUE;
      case 631 : snprintf(result,max_size,"%05d (Security Enabled Global Group Created)",eventId);break;
      case 632 : snprintf(result,max_size,"%05d (Security Enabled Global Group Member Added)",eventId);break;
      case 633 : snprintf(result,max_size,"%05d (Security Enabled Global Group Member Removed)",eventId);break;
      case 634 : snprintf(result,max_size,"%05d (Security Enabled Global Group Deleted)",eventId);break;
      case 635 : snprintf(result,max_size,"%05d (Security Enabled Local Group Created)",eventId);break;
      case 636 : snprintf(result,max_size,"%05d (Security Enabled Local Group Member Added)",eventId);break;
      case 637 : snprintf(result,max_size,"%05d (Security Enabled Local Group Member Removed)",eventId);break;
      case 638 : snprintf(result,max_size,"%05d (Security Enabled Local Group Deleted)",eventId);break;
      case 639 : snprintf(result,max_size,"%05d (Security Enabled Local Group Changed)",eventId);break;
      case 640 : snprintf(result,max_size,"%05d (General Account Database Change)",eventId);break;
      case 641 : snprintf(result,max_size,"%05d (Security Enabled Global Group Changed)",eventId);break;
      case 642 : snprintf(result,max_size,"%05d (User Account Changed)",eventId);return TRUE;
      case 643 : snprintf(result,max_size,"%05d (Domain Policy Changed)",eventId);return TRUE;
      case 644 : snprintf(result,max_size,"%05d (User Account Locked Out)",eventId);return TRUE;
      case 645 : snprintf(result,max_size,"%05d (Computer Account Created)",eventId);return TRUE;
      case 646 : snprintf(result,max_size,"%05d (Computer Account Changed)",eventId);return TRUE;
      case 647 : snprintf(result,max_size,"%05d (Computer Account Deleted)",eventId);return TRUE;
      case 648 : snprintf(result,max_size,"%05d (Security Disabled Local Group Created)",eventId);break;
      case 649 : snprintf(result,max_size,"%05d (Security Disabled Local Group Changed)",eventId);break;
      case 650 : snprintf(result,max_size,"%05d (Security Disabled Local Group Member Added)",eventId);break;
      case 651 : snprintf(result,max_size,"%05d (Security Disabled Local Group Member Removed)",eventId);break;
      case 652 : snprintf(result,max_size,"%05d (Security Disabled Local Group Deleted)",eventId);break;
      case 653 : snprintf(result,max_size,"%05d (Security Disabled Global Group Created)",eventId);break;
      case 654 : snprintf(result,max_size,"%05d (Security Disabled Global Group Changed)",eventId);break;
      case 655 : snprintf(result,max_size,"%05d (Security Disabled Global Group Member Added)",eventId);break;
      case 656 : snprintf(result,max_size,"%05d (Security Disabled Global Group Member Removed)",eventId);break;
      case 657 : snprintf(result,max_size,"%05d (Security Disabled Global Group Deleted)",eventId);break;
      case 658 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Created)",eventId);break;
      case 659 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Changed)",eventId);break;
      case 660 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Member Added)",eventId);break;
      case 661 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Member Removed)",eventId);break;
      case 662 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Deleted)",eventId);break;
      case 663 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Created)",eventId);break;
      case 664 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Changed)",eventId);break;
      case 665 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Member Added)",eventId);break;
      case 666 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Member Removed)",eventId);break;
      case 667 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Deleted)",eventId);break;
      case 668 : snprintf(result,max_size,"%05d (Group Type Changed)",eventId);break;
      case 669 : snprintf(result,max_size,"%05d (Add SID History)",eventId);break;
      case 670 : snprintf(result,max_size,"%05d (Add SID History)",eventId);break;
      case 671 : snprintf(result,max_size,"%05d (User Account Unlocked)",eventId);return TRUE;
      case 672 : snprintf(result,max_size,"%05d (Authentication Ticket Granted)",eventId);break;
      case 673 : snprintf(result,max_size,"%05d (Service Ticket Granted)",eventId);break;
      case 674 : snprintf(result,max_size,"%05d (Ticket Granted Renewed)",eventId);break;
      case 675 : snprintf(result,max_size,"%05d (Pre-authentication failed)",eventId);break;
      case 676 : snprintf(result,max_size,"%05d (Authentication Ticket Request Failed)",eventId);break;
      case 677 : snprintf(result,max_size,"%05d (Service Ticket Request Failed)",eventId);break;
      case 678 : snprintf(result,max_size,"%05d (Account Mapped for Logon by)",eventId);break;
      case 679 : snprintf(result,max_size,"%05d (The name: #2 could not be mapped for logon by: #1)",eventId);break;
      case 680 : snprintf(result,max_size,"%05d (Account Used for Logon by)",eventId);return TRUE;
      case 681 : snprintf(result,max_size,"%05d (The logon to account: #2 by: #1 from workstation: #3 failed)",eventId);return TRUE;
      case 682 : snprintf(result,max_size,"%05d (Session reconnected to winstation)",eventId);return TRUE;
      case 683 : snprintf(result,max_size,"%05d (Session disconnected from winstation)",eventId);return TRUE;
      case 684 : snprintf(result,max_size,"%05d (Set ACLs of members in administrators groups)",eventId);break;
      case 685 : snprintf(result,max_size,"%05d (Account Name Changed)",eventId);break;
      case 686 : snprintf(result,max_size,"%05d (Password of the following user accessed)",eventId);break;
      case 687 : snprintf(result,max_size,"%05d (Basic Application Group Created)",eventId);break;
      case 688 : snprintf(result,max_size,"%05d (Basic Application Group Changed)",eventId);break;
      case 689 : snprintf(result,max_size,"%05d (Basic Application Group Member Added)",eventId);break;
      case 690 : snprintf(result,max_size,"%05d (Basic Application Group Member Removed)",eventId);break;
      case 691 : snprintf(result,max_size,"%05d (Basic Application Group Non-Member Added)",eventId);break;
      case 692 : snprintf(result,max_size,"%05d (Basic Application Group Non-Member Removed)",eventId);break;
      case 693 : snprintf(result,max_size,"%05d (Basic Application Group Deleted)",eventId);break;
      case 694 : snprintf(result,max_size,"%05d (LDAP Query Group Created)",eventId);break;
      case 695 : snprintf(result,max_size,"%05d (LDAP Query Group Changed)",eventId);break;
      case 696 : snprintf(result,max_size,"%05d (LDAP Query Group Deleted)",eventId);break;
      case 697 : snprintf(result,max_size,"%05d (Password Policy Checking API is called)",eventId);break;
      case 806 : snprintf(result,max_size,"%05d (Per User Audit Policy was refreshed)",eventId);break;
      case 807 : snprintf(result,max_size,"%05d (Per user auditing policy set for user)",eventId);break;
      case 808 : snprintf(result,max_size,"%05d (A security event source has attempted to register)",eventId);break;
      case 809 : snprintf(result,max_size,"%05d (A security event source has attempted to unregister)",eventId);break;
      case 848 : snprintf(result,max_size,"%05d (The following policy was active when the Windows Firewall started)",eventId);break;
      case 849 : snprintf(result,max_size,"%05d (An application was listed as an exception when the Windows Firewall started)",eventId);break;
      case 850 : snprintf(result,max_size,"%05d (A port was listed as an exception when the Windows Firewall started)",eventId);break;
      case 852 : snprintf(result,max_size,"%05d (A change has been made to the Windows Firewall port exception list)",eventId);break;
      case 861 : snprintf(result,max_size,"%05d (The Windows Firewall has detected an application listening for incoming traffic)",eventId);return TRUE;
      case 1100 : snprintf(result,max_size,"%05d (The event logging service has shut down)",eventId);return TRUE;
      case 1101 : snprintf(result,max_size,"%05d (Audit events have been dropped by the transport)",eventId);break;
      case 1102 : snprintf(result,max_size,"%05d (The audit log was cleared)",eventId);return TRUE;
      case 1104 : snprintf(result,max_size,"%05d (The security Log is now full)",eventId);return TRUE;
      case 1105 : snprintf(result,max_size,"%05d (Event log automatic backup)",eventId);break;
      case 1108 : snprintf(result,max_size,"%05d (The event logging service encountered an error )",eventId);return TRUE;
      case 1001 : snprintf(result,max_size,"%05d (Failed during request for component)",eventId);break;
      case 1004 : snprintf(result,max_size,"%05d (Installation failed)",eventId);return TRUE;
      case 1005 : snprintf(result,max_size,"%05d (Install operation initiated a reboot)",eventId);return TRUE;
      case 1015 : snprintf(result,max_size,"%05d (TraceLevel parameter not located in registry; Default trace level used is 32)",eventId);break;
      case 1022 : snprintf(result,max_size,"%05d (Update installed successfully)",eventId);break;
      case 1023 : snprintf(result,max_size,"%05d (Update could not be installed)",eventId);break;
      case 1517 : snprintf(result,max_size,"%05d (Windows saved user registry while an application or service was still using the registry during log off. The memory used by the user's registry has not been freed)",eventId);break;
      case 2444 : snprintf(result,max_size,"%05d (MS DTC started)",eventId);break;
      case 3006 : snprintf(result,max_size,"%05d (Error reading log event record. Handle specified is 619064. Return code from ReadEventLog is)",eventId);break;
      case 3260 : snprintf(result,max_size,"%05d (This computer has been successfully joined domain)",eventId);break;
      case 4097 : snprintf(result,max_size,"%05d (Error exception)",eventId);break;
      case 4201 : snprintf(result,max_size,"%05d (New network connection)",eventId);return TRUE;
      case 4321 : snprintf(result,max_size,"%05d (Name registry of ip#1 to ip#2 fail)",eventId);break;
      case 4500 : snprintf(result,max_size,"%05d (Metabase Add Key)",eventId);break;
      case 4501 : snprintf(result,max_size,"%05d (Metabase Delete Key)",eventId);break;
      case 4502 : snprintf(result,max_size,"%05d (Metabase Delete Chid Keys)",eventId);break;
      case 4503 : snprintf(result,max_size,"%05d (Metabase Copy Key)",eventId);break;
      case 4504 : snprintf(result,max_size,"%05d (Metabase Rename Key)",eventId);break;
      case 4505 : snprintf(result,max_size,"%05d (Metabase Set Data)",eventId);break;
      case 4506 : snprintf(result,max_size,"%05d (Metabase Delete Data)",eventId);break;
      case 4507 : snprintf(result,max_size,"%05d (Metabase Delete All Data)",eventId);break;
      case 4508 : snprintf(result,max_size,"%05d (Metabase Copy Data)",eventId);break;
      case 4509 : snprintf(result,max_size,"%05d (Metabase Set Last Change Time)",eventId);break;
      case 4510 : snprintf(result,max_size,"%05d (Metabase Restore)",eventId);break;
      case 4511 : snprintf(result,max_size,"%05d (Metabase Delete Backup)",eventId);break;
      case 4512 : snprintf(result,max_size,"%05d (Metabase Import)",eventId);break;
      case 4608 : snprintf(result,max_size,"%05d (Windows is starting up)",eventId);return TRUE;
      case 4609 : snprintf(result,max_size,"%05d (Windows is shutting down)",eventId);return TRUE;
      case 4610 : snprintf(result,max_size,"%05d (An authentication package has been loaded by the Local Security Authority)",eventId);break;
      case 4611 : snprintf(result,max_size,"%05d (A trusted logon process has been registered with the Local Security Authority)",eventId);break;
      case 4612 : snprintf(result,max_size,"%05d (Internal resources allocated for the queuing of audit messages have been exhausted, leading to the loss of some audits)",eventId);break;
      case 4614 : snprintf(result,max_size,"%05d (A notification package has been loaded by the Security Account Manager)",eventId);break;
      case 4615 : snprintf(result,max_size,"%05d (Invalid use of LPC port)",eventId);break;
      case 4616 : snprintf(result,max_size,"%05d (The system time was changed)",eventId);return TRUE;
      case 4618 : snprintf(result,max_size,"%05d (A monitored security event pattern has occurred)",eventId);break;
      case 4621 : snprintf(result,max_size,"%05d (Administrator recovered system from CrashOnAuditFail)",eventId);break;
      case 4622 : snprintf(result,max_size,"%05d (A security package has been loaded by the Local Security Authority)",eventId);break;
      case 4624 : snprintf(result,max_size,"%05d (An account was successfully logged on)",eventId);return TRUE;
      case 4625 : snprintf(result,max_size,"%05d (An account failed to log on)",eventId);return TRUE;
      case 4634 : snprintf(result,max_size,"%05d (An account was logged off)",eventId);return TRUE;
      case 4646 : snprintf(result,max_size,"%05d (IKE DoS-prevention mode started)",eventId);break;
      case 4647 : snprintf(result,max_size,"%05d (User initiated logoff)",eventId);break;
      case 4648 : snprintf(result,max_size,"%05d (A logon was attempted using explicit credentials)",eventId);break;
      case 4649 : snprintf(result,max_size,"%05d (A replay attack was detected)",eventId);break;
      case 4650 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association was established)",eventId);break;
      case 4651 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association was established)",eventId);break;
      case 4652 : snprintf(result,max_size,"%05d (An IPsec Main Mode negotiation failed)",eventId);break;
      case 4653 : snprintf(result,max_size,"%05d (An IPsec Main Mode negotiation failed)",eventId);break;
      case 4654 : snprintf(result,max_size,"%05d (An IPsec Quick Mode negotiation failed)",eventId);break;
      case 4655 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association ended)",eventId);break;
      case 4656 : snprintf(result,max_size,"%05d (A handle to an object was requested)",eventId);break;
      case 4657 : snprintf(result,max_size,"%05d (A registry value was modified)",eventId);break;
      case 4658 : snprintf(result,max_size,"%05d (The handle to an object was closed)",eventId);break;
      case 4659 : snprintf(result,max_size,"%05d (A handle to an object was requested with intent to delete)",eventId);break;
      case 4660 : snprintf(result,max_size,"%05d (An object was deleted)",eventId);break;
      case 4661 : snprintf(result,max_size,"%05d (A handle to an object was requested)",eventId);break;
      case 4662 : snprintf(result,max_size,"%05d (An operation was performed on an object)",eventId);break;
      case 4663 : snprintf(result,max_size,"%05d (An attempt was made to access an object)",eventId);break;
      case 4664 : snprintf(result,max_size,"%05d (An attempt was made to create a hard link)",eventId);break;
      case 4665 : snprintf(result,max_size,"%05d (An attempt was made to create an application client context)",eventId);break;
      case 4666 : snprintf(result,max_size,"%05d (An application attempted an operation)",eventId);break;
      case 4667 : snprintf(result,max_size,"%05d (An application client context was deleted)",eventId);break;
      case 4668 : snprintf(result,max_size,"%05d (An application was initialized)",eventId);break;
      case 4670 : snprintf(result,max_size,"%05d (Permissions on an object were changed)",eventId);break;
      case 4671 : snprintf(result,max_size,"%05d (An application attempted to access a blocked ordinal through the TBS)",eventId);break;
      case 4672 : snprintf(result,max_size,"%05d (Special privileges assigned to new logon)",eventId);break;
      case 4673 : snprintf(result,max_size,"%05d (A privileged service was called)",eventId);break;
      case 4674 : snprintf(result,max_size,"%05d (An operation was attempted on a privileged object)",eventId);break;
      case 4675 : snprintf(result,max_size,"%05d (SIDs were filtered)",eventId);break;
      case 4688 : snprintf(result,max_size,"%05d (A new process has been created)",eventId);break;
      case 4689 : snprintf(result,max_size,"%05d (A process has exited)",eventId);break;
      case 4690 : snprintf(result,max_size,"%05d (An attempt was made to duplicate a handle to an object)",eventId);break;
      case 4691 : snprintf(result,max_size,"%05d (Indirect access to an object was requested)",eventId);break;
      case 4692 : snprintf(result,max_size,"%05d (Backup of data protection master key was attempted)",eventId);break;
      case 4693 : snprintf(result,max_size,"%05d (Recovery of data protection master key was attempted)",eventId);break;
      case 4694 : snprintf(result,max_size,"%05d (Protection of auditable protected data was attempted)",eventId);break;
      case 4695 : snprintf(result,max_size,"%05d (Unprotection of auditable protected data was attempted)",eventId);break;
      case 4696 : snprintf(result,max_size,"%05d (A primary token was assigned to process)",eventId);break;
      case 4697 : snprintf(result,max_size,"%05d (A service was installed in the system)",eventId);break;
      case 4698 : snprintf(result,max_size,"%05d (A scheduled task was created)",eventId);break;
      case 4699 : snprintf(result,max_size,"%05d (A scheduled task was deleted)",eventId);break;
      case 4700 : snprintf(result,max_size,"%05d (A scheduled task was enabled)",eventId);break;
      case 4701 : snprintf(result,max_size,"%05d (A scheduled task was disabled)",eventId);break;
      case 4702 : snprintf(result,max_size,"%05d (A scheduled task was updated)",eventId);break;
      case 4704 : snprintf(result,max_size,"%05d (A user right was assigned)",eventId);break;
      case 4705 : snprintf(result,max_size,"%05d (A user right was removed)",eventId);break;
      case 4706 : snprintf(result,max_size,"%05d (A new trust was created to a domain)",eventId);break;
      case 4707 : snprintf(result,max_size,"%05d (A trust to a domain was removed)",eventId);break;
      case 4709 : snprintf(result,max_size,"%05d (IPsec Services was started)",eventId);break;
      case 4710 : snprintf(result,max_size,"%05d (IPsec Services was disabled)",eventId);break;
      case 4711 : snprintf(result,max_size,"%05d (PAStore Engine (#1))",eventId);break;
      case 4712 : snprintf(result,max_size,"%05d (IPsec Services encountered a potentially serious failure)",eventId);break;
      case 4713 : snprintf(result,max_size,"%05d (Kerberos policy was changed)",eventId);break;
      case 4714 : snprintf(result,max_size,"%05d (Encrypted data recovery policy was changed)",eventId);break;
      case 4715 : snprintf(result,max_size,"%05d (The audit policy (SACL) on an object was changed)",eventId);break;
      case 4716 : snprintf(result,max_size,"%05d (Trusted domain information was modified)",eventId);break;
      case 4717 : snprintf(result,max_size,"%05d (System security access was granted to an account)",eventId);break;
      case 4718 : snprintf(result,max_size,"%05d (System security access was removed from an account)",eventId);break;
      case 4719 : snprintf(result,max_size,"%05d (System audit policy was changed)",eventId);break;
      case 4720 : snprintf(result,max_size,"%05d (A user account was created)",eventId);return TRUE;
      case 4722 : snprintf(result,max_size,"%05d (A user account was enabled)",eventId);return TRUE;
      case 4723 : snprintf(result,max_size,"%05d (An attempt was made to change an account's password)",eventId);return TRUE;
      case 4724 : snprintf(result,max_size,"%05d (An attempt was made to reset an accounts password)",eventId);return TRUE;
      case 4725 : snprintf(result,max_size,"%05d (A user account was disabled)",eventId);return TRUE;
      case 4726 : snprintf(result,max_size,"%05d (A user account was deleted)",eventId);return TRUE;
      case 4727 : snprintf(result,max_size,"%05d (A security-enabled global group was created)",eventId);break;
      case 4728 : snprintf(result,max_size,"%05d (A member was added to a security-enabled global group)",eventId);break;
      case 4729 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled global group)",eventId);break;
      case 4730 : snprintf(result,max_size,"%05d (A security-enabled global group was deleted)",eventId);break;
      case 4731 : snprintf(result,max_size,"%05d (A security-enabled local group was created)",eventId);break;
      case 4732 : snprintf(result,max_size,"%05d (A member was added to a security-enabled local group)",eventId);break;
      case 4733 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled local group)",eventId);break;
      case 4734 : snprintf(result,max_size,"%05d (A security-enabled local group was deleted)",eventId);break;
      case 4735 : snprintf(result,max_size,"%05d (A security-enabled local group was changed)",eventId);break;
      case 4737 : snprintf(result,max_size,"%05d (A security-enabled global group was changed)",eventId);break;
      case 4738 : snprintf(result,max_size,"%05d (A user account was changed)",eventId);return TRUE;
      case 4739 : snprintf(result,max_size,"%05d (Domain Policy was changed)",eventId);break;
      case 4740 : snprintf(result,max_size,"%05d (A user account was locked out)",eventId);return TRUE;
      case 4741 : snprintf(result,max_size,"%05d (A computer account was created)",eventId);return TRUE;
      case 4742 : snprintf(result,max_size,"%05d (A computer account was changed)",eventId);return TRUE;
      case 4743 : snprintf(result,max_size,"%05d (A computer account was deleted)",eventId);return TRUE;
      case 4744 : snprintf(result,max_size,"%05d (A security-disabled local group was created)",eventId);break;
      case 4745 : snprintf(result,max_size,"%05d (A security-disabled local group was changed)",eventId);break;
      case 4746 : snprintf(result,max_size,"%05d (A member was added to a security-disabled local group)",eventId);break;
      case 4747 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled local group)",eventId);break;
      case 4748 : snprintf(result,max_size,"%05d (A security-disabled local group was deleted)",eventId);break;
      case 4749 : snprintf(result,max_size,"%05d (A security-disabled global group was created)",eventId);break;
      case 4750 : snprintf(result,max_size,"%05d (A security-disabled global group was changed)",eventId);break;
      case 4751 : snprintf(result,max_size,"%05d (A member was added to a security-disabled global group)",eventId);break;
      case 4752 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled global group)",eventId);break;
      case 4753 : snprintf(result,max_size,"%05d (A security-disabled global group was deleted)",eventId);break;
      case 4754 : snprintf(result,max_size,"%05d (A security-enabled universal group was created)",eventId);break;
      case 4755 : snprintf(result,max_size,"%05d (A security-enabled universal group was changed)",eventId);break;
      case 4756 : snprintf(result,max_size,"%05d (A member was added to a security-enabled universal group)",eventId);break;
      case 4757 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled universal group)",eventId);break;
      case 4758 : snprintf(result,max_size,"%05d (A security-enabled universal group was deleted)",eventId);break;
      case 4759 : snprintf(result,max_size,"%05d (A security-disabled universal group was created)",eventId);break;
      case 4760 : snprintf(result,max_size,"%05d (A security-disabled universal group was changed)",eventId);break;
      case 4761 : snprintf(result,max_size,"%05d (A member was added to a security-disabled universal group)",eventId);break;
      case 4762 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled universal group)",eventId);break;
      case 4763 : snprintf(result,max_size,"%05d (A security-disabled universal group was deleted)",eventId);break;
      case 4764 : snprintf(result,max_size,"%05d (A groups type was changed)",eventId);break;
      case 4765 : snprintf(result,max_size,"%05d (SID History was added to an account)",eventId);break;
      case 4766 : snprintf(result,max_size,"%05d (An attempt to add SID History to an account failed)",eventId);break;
      case 4767 : snprintf(result,max_size,"%05d (A user account was unlocked)",eventId);return TRUE;
      case 4768 : snprintf(result,max_size,"%05d (A Kerberos authentication ticket (TGT) was requested)",eventId);break;
      case 4769 : snprintf(result,max_size,"%05d (A Kerberos service ticket was requested)",eventId);break;
      case 4770 : snprintf(result,max_size,"%05d (A Kerberos service ticket was renewed)",eventId);break;
      case 4771 : snprintf(result,max_size,"%05d (Kerberos pre-authentication failed)",eventId);break;
      case 4772 : snprintf(result,max_size,"%05d (A Kerberos authentication ticket request failed)",eventId);break;
      case 4773 : snprintf(result,max_size,"%05d (A Kerberos service ticket request failed)",eventId);break;
      case 4774 : snprintf(result,max_size,"%05d (An account was mapped for logon)",eventId);return TRUE;
      case 4775 : snprintf(result,max_size,"%05d (An account could not be mapped for logon)",eventId);return TRUE;
      case 4776 : snprintf(result,max_size,"%05d (The domain controller attempted to validate the credentials for an account)",eventId);break;
      case 4777 : snprintf(result,max_size,"%05d (The domain controller failed to validate the credentials for an account)",eventId);break;
      case 4778 : snprintf(result,max_size,"%05d (A session was reconnected to a Window Station)",eventId);return TRUE;
      case 4779 : snprintf(result,max_size,"%05d (A session was disconnected from a Window Station)",eventId);return TRUE;
      case 4780 : snprintf(result,max_size,"%05d (The ACL was set on accounts which are members of administrators groups)",eventId);break;
      case 4781 : snprintf(result,max_size,"%05d (The name of an account was changed)",eventId);break;
      case 4782 : snprintf(result,max_size,"%05d (The password hash an account was accessed)",eventId);break;
      case 4783 : snprintf(result,max_size,"%05d (A basic application group was created)",eventId);break;
      case 4784 : snprintf(result,max_size,"%05d (A basic application group was changed)",eventId);break;
      case 4785 : snprintf(result,max_size,"%05d (A member was added to a basic application group)",eventId);break;
      case 4786 : snprintf(result,max_size,"%05d (A member was removed from a basic application group)",eventId);break;
      case 4787 : snprintf(result,max_size,"%05d (A non-member was added to a basic application group)",eventId);break;
      case 4788 : snprintf(result,max_size,"%05d (A non-member was removed from a basic application group.)",eventId);break;
      case 4789 : snprintf(result,max_size,"%05d (A basic application group was deleted)",eventId);break;
      case 4790 : snprintf(result,max_size,"%05d (An LDAP query group was created)",eventId);break;
      case 4791 : snprintf(result,max_size,"%05d (A basic application group was changed)",eventId);break;
      case 4792 : snprintf(result,max_size,"%05d (An LDAP query group was deleted)",eventId);break;
      case 4793 : snprintf(result,max_size,"%05d (The Password Policy Checking API was called)",eventId);break;
      case 4794 : snprintf(result,max_size,"%05d (An attempt was made to set the Directory Services Restore Mode administrator password)",eventId);break;
      case 4800 : snprintf(result,max_size,"%05d (The workstation was locked)",eventId);return TRUE;
      case 4801 : snprintf(result,max_size,"%05d (The workstation was unlocked)",eventId);return TRUE;
      case 4802 : snprintf(result,max_size,"%05d (The screen saver was invoked)",eventId);break;
      case 4803 : snprintf(result,max_size,"%05d (The screen saver was dismissed)",eventId);break;
      case 4816 : snprintf(result,max_size,"%05d (RPC detected an integrity violation while decrypting an incoming message)",eventId);break;
      case 4817 : snprintf(result,max_size,"%05d (Auditing settings on object were changed)",eventId);break;
      case 4864 : snprintf(result,max_size,"%05d (A namespace collision was detected)",eventId);break;
      case 4865 : snprintf(result,max_size,"%05d (A trusted forest information entry was added)",eventId);break;
      case 4866 : snprintf(result,max_size,"%05d (A trusted forest information entry was removed)",eventId);break;
      case 4867 : snprintf(result,max_size,"%05d (A trusted forest information entry was modified)",eventId);break;
      case 4868 : snprintf(result,max_size,"%05d (The certificate manager denied a pending certificate request)",eventId);break;
      case 4869 : snprintf(result,max_size,"%05d (Certificate Services received a resubmitted certificate request)",eventId);break;
      case 4870 : snprintf(result,max_size,"%05d (Certificate Services revoked a certificate)",eventId);break;
      case 4871 : snprintf(result,max_size,"%05d (Certificate Services received a request to publish the certificate revocation list (CRL))",eventId);break;
      case 4872 : snprintf(result,max_size,"%05d (Certificate Services published the certificate revocation list (CRL))",eventId);break;
      case 4873 : snprintf(result,max_size,"%05d (A certificate request extension changed)",eventId);break;
      case 4874 : snprintf(result,max_size,"%05d (One or more certificate request attributes changed)",eventId);break;
      case 4875 : snprintf(result,max_size,"%05d (Certificate Services received a request to shut down)",eventId);break;
      case 4876 : snprintf(result,max_size,"%05d (Certificate Services backup started)",eventId);break;
      case 4877 : snprintf(result,max_size,"%05d (Certificate Services backup completed)",eventId);break;
      case 4878 : snprintf(result,max_size,"%05d (Certificate Services restore started)",eventId);break;
      case 4879 : snprintf(result,max_size,"%05d (Certificate Services restore completed)",eventId);break;
      case 4880 : snprintf(result,max_size,"%05d (Certificate Services started)",eventId);break;
      case 4881 : snprintf(result,max_size,"%05d (Certificate Services stopped)",eventId);break;
      case 4882 : snprintf(result,max_size,"%05d (The security permissions for Certificate Services changed)",eventId);break;
      case 4883 : snprintf(result,max_size,"%05d (Certificate Services retrieved an archived key)",eventId);break;
      case 4884 : snprintf(result,max_size,"%05d (Certificate Services imported a certificate into its database)",eventId);break;
      case 4885 : snprintf(result,max_size,"%05d (The audit filter for Certificate Services changed)",eventId);break;
      case 4886 : snprintf(result,max_size,"%05d (Certificate Services received a certificate request)",eventId);break;
      case 4887 : snprintf(result,max_size,"%05d (Certificate Services approved a certificate request and issued a certificate)",eventId);break;
      case 4888 : snprintf(result,max_size,"%05d (Certificate Services denied a certificate request)",eventId);break;
      case 4889 : snprintf(result,max_size,"%05d (Certificate Services set the status of a certificate request to pending)",eventId);break;
      case 4890 : snprintf(result,max_size,"%05d (The certificate manager settings for Certificate Services changed)",eventId);break;
      case 4891 : snprintf(result,max_size,"%05d (A configuration entry changed in Certificate Services)",eventId);break;
      case 4892 : snprintf(result,max_size,"%05d (A property of Certificate Services changed)",eventId);break;
      case 4893 : snprintf(result,max_size,"%05d (Certificate Services archived a key)",eventId);break;
      case 4894 : snprintf(result,max_size,"%05d (Certificate Services imported and archived a key)",eventId);break;
      case 4895 : snprintf(result,max_size,"%05d (Certificate Services published the CA certificate to Active Directory Domain Services)",eventId);break;
      case 4896 : snprintf(result,max_size,"%05d (One or more rows have been deleted from the certificate database)",eventId);break;
      case 4897 : snprintf(result,max_size,"%05d (Role separation enabled)",eventId);break;
      case 4898 : snprintf(result,max_size,"%05d (Certificate Services loaded a template)",eventId);break;
      case 4899 : snprintf(result,max_size,"%05d (A Certificate Services template was updated)",eventId);break;
      case 4900 : snprintf(result,max_size,"%05d (Certificate Services template security was updated)",eventId);break;
      case 4902 : snprintf(result,max_size,"%05d (The Per-user audit policy table was created)",eventId);break;
      case 4904 : snprintf(result,max_size,"%05d (An attempt was made to register a security event source)",eventId);break;
      case 4905 : snprintf(result,max_size,"%05d (An attempt was made to unregister a security event source)",eventId);break;
      case 4906 : snprintf(result,max_size,"%05d (The CrashOnAuditFail value has changed)",eventId);break;
      case 4907 : snprintf(result,max_size,"%05d (Auditing settings on object were changed)",eventId);break;
      case 4908 : snprintf(result,max_size,"%05d (Special Groups Logon table modified)",eventId);break;
      case 4909 : snprintf(result,max_size,"%05d (The local policy settings for the TBS were changed)",eventId);break;
      case 4910 : snprintf(result,max_size,"%05d (The group policy settings for the TBS were changed)",eventId);break;
      case 4912 : snprintf(result,max_size,"%05d (Per User Audit Policy was changed)",eventId);break;
      case 4928 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was established)",eventId);break;
      case 4929 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was removed)",eventId);break;
      case 4930 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was modified)",eventId);break;
      case 4931 : snprintf(result,max_size,"%05d (An Active Directory replica destination naming context was modified)",eventId);break;
      case 4932 : snprintf(result,max_size,"%05d (Synchronization of a replica of an Active Directory naming context has begun)",eventId);break;
      case 4933 : snprintf(result,max_size,"%05d (Synchronization of a replica of an Active Directory naming context has ended)",eventId);break;
      case 4934 : snprintf(result,max_size,"%05d (Attributes of an Active Directory object were replicated)",eventId);break;
      case 4935 : snprintf(result,max_size,"%05d (Replication failure begins)",eventId);break;
      case 4936 : snprintf(result,max_size,"%05d (Replication failure ends)",eventId);break;
      case 4937 : snprintf(result,max_size,"%05d (A lingering object was removed from a replica)",eventId);break;
      case 4944 : snprintf(result,max_size,"%05d (The following policy was active when the Windows Firewall started)",eventId);break;
      case 4945 : snprintf(result,max_size,"%05d (A rule was listed when the Windows Firewall started)",eventId);break;
      case 4946 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was added)",eventId);break;
      case 4947 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was modified)",eventId);break;
      case 4948 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was deleted)",eventId);break;
      case 4949 : snprintf(result,max_size,"%05d (Windows Firewall settings were restored to the default values)",eventId);return TRUE;
      case 4950 : snprintf(result,max_size,"%05d (A Windows Firewall setting has changed)",eventId);return TRUE;
      case 4951 : snprintf(result,max_size,"%05d (A rule has been ignored because its major version number was not recognized by Windows Firewall)",eventId);break;
      case 4952 : snprintf(result,max_size,"%05d (Parts of a rule have been ignored because its minor version number was not recognized by Windows Firewall)",eventId);break;
      case 4953 : snprintf(result,max_size,"%05d (A rule has been ignored by Windows Firewall because it could not parse the rule)",eventId);break;
      case 4954 : snprintf(result,max_size,"%05d (Windows Firewall Group Policy settings has changed. The new settings have been applied)",eventId);break;
      case 4956 : snprintf(result,max_size,"%05d (Windows Firewall has changed the active profile)",eventId);break;
      case 4957 : snprintf(result,max_size,"%05d (Windows Firewall did not apply the following rule)",eventId);break;
      case 4958 : snprintf(result,max_size,"%05d (Windows Firewall did not apply the following rule because the rule referred to items not configured on this computer)",eventId);break;
      case 4960 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed an integrity check)",eventId);break;
      case 4961 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed a replay check)",eventId);break;
      case 4962 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed a replay check)",eventId);break;
      case 4963 : snprintf(result,max_size,"%05d (IPsec dropped an inbound clear text packet that should have been secured)",eventId);break;
      case 4964 : snprintf(result,max_size,"%05d (Special groups have been assigned to a new logon)",eventId);break;
      case 4965 : snprintf(result,max_size,"%05d (IPsec received a packet from a remote computer with an incorrect Security Parameter Index (SPI))",eventId);break;
      case 4976 : snprintf(result,max_size,"%05d (During Main Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4977 : snprintf(result,max_size,"%05d (During Quick Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4978 : snprintf(result,max_size,"%05d (During Extended Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4979 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4980 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4981 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4982 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4983 : snprintf(result,max_size,"%05d (An IPsec Extended Mode negotiation failed)",eventId);break;
      case 4984 : snprintf(result,max_size,"%05d (An IPsec Extended Mode negotiation failed)",eventId);break;
      case 4985 : snprintf(result,max_size,"%05d (The state of a transaction has changed)",eventId);break;
      case 5000 : snprintf(result,max_size,"%05d (Installation error)",eventId);break;
      case 5024 : snprintf(result,max_size,"%05d (The Windows Firewall Service has started successfully)",eventId);return TRUE;
      case 5025 : snprintf(result,max_size,"%05d (The Windows Firewall Service has been stopped)",eventId);return TRUE;
      case 5027 : snprintf(result,max_size,"%05d (The Windows Firewall Service was unable to retrieve the security policy from the local storage)",eventId);break;
      case 5028 : snprintf(result,max_size,"%05d (The Windows Firewall Service was unable to parse the new security policy)",eventId);break;
      case 5029 : snprintf(result,max_size,"%05d (The Windows Firewall Service failed to initialize the driver)",eventId);return TRUE;
      case 5030 : snprintf(result,max_size,"%05d (The Windows Firewall Service failed to start)",eventId);break;
      case 5031 : snprintf(result,max_size,"%05d (The Windows Firewall Service blocked an application from accepting incoming connections on the network)",eventId);break;
      case 5032 : snprintf(result,max_size,"%05d (Windows Firewall was unable to notify the user that it blocked an application from accepting incoming connections on the network)",eventId);break;
      case 5033 : snprintf(result,max_size,"%05d (The Windows Firewall Driver has started successfully)",eventId);break;
      case 5034 : snprintf(result,max_size,"%05d (The Windows Firewall Driver has been stopped)",eventId);return TRUE;
      case 5035 : snprintf(result,max_size,"%05d (The Windows Firewall Driver failed to start)",eventId);break;
      case 5037 : snprintf(result,max_size,"%05d (The Windows Firewall Driver detected critical runtime error. Terminating)",eventId);break;
      case 5038 : snprintf(result,max_size,"%05d (Code integrity determined that the image hash of a file is not valid)",eventId);break;
      case 5039 : snprintf(result,max_size,"%05d (A registry key was virtualized)",eventId);break;
      case 5040 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was added)",eventId);break;
      case 5041 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was modified)",eventId);break;
      case 5042 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was deleted)",eventId);break;
      case 5043 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was added)",eventId);break;
      case 5044 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was modified)",eventId);break;
      case 5045 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was deleted)",eventId);break;
      case 5046 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was added)",eventId);break;
      case 5047 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was modified)",eventId);break;
      case 5048 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was deleted)",eventId);break;
      case 5049 : snprintf(result,max_size,"%05d (An IPsec Security Association was deleted)",eventId);break;
      case 5050 : snprintf(result,max_size,"%05d (An attempt to programmatically disable the Windows Firewall using a call to INetFwProfile.FirewallEnabled(FALSE)",eventId);return TRUE;
      case 5051 : snprintf(result,max_size,"%05d (A file was virtualized)",eventId);break;
      case 5056 : snprintf(result,max_size,"%05d (A cryptographic self test was performed)",eventId);break;
      case 5057 : snprintf(result,max_size,"%05d (A cryptographic primitive operation failed)",eventId);break;
      case 5058 : snprintf(result,max_size,"%05d (Key file operation)",eventId);break;
      case 5059 : snprintf(result,max_size,"%05d (Key migration operation)",eventId);break;
      case 5060 : snprintf(result,max_size,"%05d (Verification operation failed)",eventId);break;
      case 5061 : snprintf(result,max_size,"%05d (Cryptographic operation)",eventId);break;
      case 5062 : snprintf(result,max_size,"%05d (A kernel-mode cryptographic self test was performed)",eventId);break;
      case 5063 : snprintf(result,max_size,"%05d (A cryptographic provider operation was attempted)",eventId);break;
      case 5064 : snprintf(result,max_size,"%05d (A cryptographic context operation was attempted)",eventId);break;
      case 5065 : snprintf(result,max_size,"%05d (A cryptographic context modification was attempted)",eventId);break;
      case 5066 : snprintf(result,max_size,"%05d (A cryptographic function operation was attempted)",eventId);break;
      case 5067 : snprintf(result,max_size,"%05d (A cryptographic function modification was attempted)",eventId);break;
      case 5068 : snprintf(result,max_size,"%05d (A cryptographic function provider operation was attempted)",eventId);break;
      case 5069 : snprintf(result,max_size,"%05d (A cryptographic function property operation was attempted)",eventId);break;
      case 5070 : snprintf(result,max_size,"%05d (A cryptographic function property operation was attempted)",eventId);break;
      case 5120 : snprintf(result,max_size,"%05d (OCSP Responder Service Started)",eventId);break;
      case 5121 : snprintf(result,max_size,"%05d (OCSP Responder Service Stopped)",eventId);break;
      case 5122 : snprintf(result,max_size,"%05d (A Configuration entry changed in the OCSP Responder Service)",eventId);break;
      case 5123 : snprintf(result,max_size,"%05d (A configuration entry changed in the OCSP Responder Service)",eventId);break;
      case 5124 : snprintf(result,max_size,"%05d (A security setting was updated on OCSP Responder Service)",eventId);break;
      case 5125 : snprintf(result,max_size,"%05d (A request was submitted to OCSP Responder Service)",eventId);break;
      case 5126 : snprintf(result,max_size,"%05d (Signing Certificate was automatically updated by the OCSP Responder Service)",eventId);break;
      case 5127 : snprintf(result,max_size,"%05d (The OCSP Revocation Provider successfully updated the revocation information)",eventId);break;
      case 5136 : snprintf(result,max_size,"%05d (A directory service object was modified)",eventId);break;
      case 5137 : snprintf(result,max_size,"%05d (A directory service object was created)",eventId);break;
      case 5138 : snprintf(result,max_size,"%05d (A directory service object was undeleted)",eventId);break;
      case 5139 : snprintf(result,max_size,"%05d (A directory service object was moved)",eventId);break;
      case 5140 : snprintf(result,max_size,"%05d (A network share object was accessed)",eventId);break;
      case 5141 : snprintf(result,max_size,"%05d (A directory service object was deleted)",eventId);break;
      case 5142 : snprintf(result,max_size,"%05d (A network share object was added)",eventId);break;
      case 5143 : snprintf(result,max_size,"%05d (A network share object was modified)",eventId);break;
      case 5144 : snprintf(result,max_size,"%05d (A network share object was deleted)",eventId);break;
      case 5145 : snprintf(result,max_size,"%05d (A network share object was checked to see whether client can be granted desired access)",eventId);break;
      case 5148 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has detected a DoS attack and entered a defensive mode; packets associated with this attack will be discarded)",eventId);return TRUE;
      case 5149 : snprintf(result,max_size,"%05d (The DoS attack has subsided and normal processing is being resumed)",eventId);return TRUE;
      case 5150 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a packet)",eventId);break;
      case 5151 : snprintf(result,max_size,"%05d (A more restrictive Windows Filtering Platform filter has blocked a packet)",eventId);break;
      case 5152 : snprintf(result,max_size,"%05d (The Windows Filtering Platform blocked a packet)",eventId);break;
      case 5153 : snprintf(result,max_size,"%05d (A more restrictive Windows Filtering Platform filter has blocked a packet)",eventId);break;
      case 5154 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has permitted an application or service to listen on a port for incoming connections)",eventId);break;
      case 5155 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked an application or service from listening on a port for incoming connections)",eventId);break;
      case 5156 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has allowed a connection)",eventId);break;
      case 5157 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a connection)",eventId);break;
      case 5158 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has permitted a bind to a local port)",eventId);break;
      case 5159 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a bind to a local port)",eventId);break;
      case 5168 : snprintf(result,max_size,"%05d (Spn check for SMB/SMB2 fails)",eventId);break;
      case 5376 : snprintf(result,max_size,"%05d (Credential Manager credentials were backed up)",eventId);break;
      case 5377 : snprintf(result,max_size,"%05d (Credential Manager credentials were restored from a backup)",eventId);break;
      case 5378 : snprintf(result,max_size,"%05d (The requested credentials delegation was disallowed by policy)",eventId);break;
      case 5440 : snprintf(result,max_size,"%05d (The following callout was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5441 : snprintf(result,max_size,"%05d (The following filter was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5442 : snprintf(result,max_size,"%05d (The following provider was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5443 : snprintf(result,max_size,"%05d (The following provider context was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5444 : snprintf(result,max_size,"%05d (The following sub-layer was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5446 : snprintf(result,max_size,"%05d (A Windows Filtering Platform callout has been changed)",eventId);break;
      case 5447 : snprintf(result,max_size,"%05d (A Windows Filtering Platform filter has been changed)",eventId);break;
      case 5448 : snprintf(result,max_size,"%05d (A Windows Filtering Platform provider has been changed)",eventId);break;
      case 5449 : snprintf(result,max_size,"%05d (A Windows Filtering Platform provider context has been changed)",eventId);break;
      case 5450 : snprintf(result,max_size,"%05d (A Windows Filtering Platform sub-layer has been changed)",eventId);break;
      case 5451 : snprintf(result,max_size,"%05d (An IPsec Quick Mode security association was established)",eventId);break;
      case 5452 : snprintf(result,max_size,"%05d (An IPsec Quick Mode security association ended)",eventId);break;
      case 5453 : snprintf(result,max_size,"%05d (An IPsec negotiation with a remote computer failed because the IKE and AuthIP IPsec Keying Modules (IKEEXT) service is not started)",eventId);break;
      case 5456 : snprintf(result,max_size,"%05d (PAStore Engine applied Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5457 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5458 : snprintf(result,max_size,"%05d (PAStore Engine applied locally cached copy of Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5459 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply locally cached copy of Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5460 : snprintf(result,max_size,"%05d (PAStore Engine applied local registry storage IPsec policy on the computer)",eventId);break;
      case 5461 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply local registry storage IPsec policy on the computer)",eventId);break;
      case 5462 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply some rules of the active IPsec policy on the computer)",eventId);break;
      case 5463 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the active IPsec policy and detected no changes)",eventId);break;
      case 5464 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the active IPsec policy, detected changes, and applied them to IPsec Services)",eventId);break;
      case 5465 : snprintf(result,max_size,"%05d (PAStore Engine received a control for forced reloading of IPsec policy and processed the control successfully)",eventId);break;
      case 5466 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory cannot be reached, and will use the cached copy of the Active Directory IPsec policy instead)",eventId);break;
      case 5467 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory can be reached, and found no changes to the policy)",eventId);break;
      case 5468 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory can be reached, found changes to the policy, and applied those changes)",eventId);break;
      case 5471 : snprintf(result,max_size,"%05d (PAStore Engine loaded local storage IPsec policy on the computer)",eventId);break;
      case 5472 : snprintf(result,max_size,"%05d (PAStore Engine failed to load local storage IPsec policy on the computer)",eventId);break;
      case 5473 : snprintf(result,max_size,"%05d (PAStore Engine loaded directory storage IPsec policy on the computer)",eventId);break;
      case 5474 : snprintf(result,max_size,"%05d (PAStore Engine failed to load directory storage IPsec policy on the computer)",eventId);break;
      case 5477 : snprintf(result,max_size,"%05d (PAStore Engine failed to add quick mode filter)",eventId);break;
      case 5478 : snprintf(result,max_size,"%05d (IPsec Services has started successfully)",eventId);break;
      case 5479 : snprintf(result,max_size,"%05d (IPsec Services has been shut down successfully)",eventId);break;
      case 5480 : snprintf(result,max_size,"%05d (IPsec Services failed to get the complete list of network interfaces on the computer)",eventId);break;
      case 5483 : snprintf(result,max_size,"%05d (IPsec Services failed to initialize RPC server. IPsec Services could not be started)",eventId);break;
      case 5484 : snprintf(result,max_size,"%05d (IPsec Services has experienced a critical failure and has been shut down)",eventId);break;
      case 5485 : snprintf(result,max_size,"%05d (IPsec Services failed to process some IPsec filters on a plug-and-play event for network interfaces)",eventId);break;
      case 5632 : snprintf(result,max_size,"%05d (A request was made to authenticate to a wireless network)",eventId);break;
      case 5633 : snprintf(result,max_size,"%05d (A request was made to authenticate to a wired network)",eventId);break;
      case 5712 : snprintf(result,max_size,"%05d (A Remote Procedure Call (RPC) was attempted)",eventId);break;
      case 5888 : snprintf(result,max_size,"%05d (An object in the COM+ Catalog was modified)",eventId);break;
      case 5889 : snprintf(result,max_size,"%05d (An object was deleted from the COM+ Catalog)",eventId);break;
      case 5890 : snprintf(result,max_size,"%05d (An object was added to the COM+ Catalog)",eventId);break;
      case 6005 : snprintf(result,max_size,"%05d (Start Event service)",eventId);return TRUE;
      case 6006 : snprintf(result,max_size,"%05d (Stop Event service)",eventId);return TRUE;
      case 6009 : snprintf(result,max_size,"%05d (Description)",eventId);break;
      case 6011 : snprintf(result,max_size,"%05d (New computer name #1 to #2)",eventId);break;
      case 6144 : snprintf(result,max_size,"%05d (Security policy in the group policy objects has been applied successfully)",eventId);break;
      case 6145 : snprintf(result,max_size,"%05d (One or more errors occured while processing security policy in the group policy objects)",eventId);break;
      case 6272 : snprintf(result,max_size,"%05d (Network Policy Server granted access to a user)",eventId);break;
      case 6273 : snprintf(result,max_size,"%05d (Network Policy Server denied access to a user)",eventId);break;
      case 6274 : snprintf(result,max_size,"%05d (Network Policy Server discarded the request for a user)",eventId);break;
      case 6275 : snprintf(result,max_size,"%05d (Network Policy Server discarded the accounting request for a user)",eventId);break;
      case 6276 : snprintf(result,max_size,"%05d (Network Policy Server quarantined a user)",eventId);break;
      case 6277 : snprintf(result,max_size,"%05d (Network Policy Server granted access to a user but put it on probation because the host did not meet the defined health policy)",eventId);break;
      case 6278 : snprintf(result,max_size,"%05d (Network Policy Server granted full access to a user because the host met the defined health policy)",eventId);break;
      case 6279 : snprintf(result,max_size,"%05d (Network Policy Server locked the user account due to repeated failed authentication attempts)",eventId);break;
      case 6280 : snprintf(result,max_size,"%05d (Network Policy Server unlocked the user account)",eventId);break;
      case 6281 : snprintf(result,max_size,"%05d (Code Integrity determined that the page hashes of an image file are not valid... )",eventId);break;
      case 6400 : snprintf(result,max_size,"%05d (BranchCache: Received an incorrectly formatted response while discovering availability of content. )",eventId);break;
      case 6401 : snprintf(result,max_size,"%05d (BranchCache: Received invalid data from a peer. Data discarded. )",eventId);break;
      case 6402 : snprintf(result,max_size,"%05d (BranchCache: The message to the hosted cache offering it data is incorrectly formatted)",eventId);break;
      case 6403 : snprintf(result,max_size,"%05d (BranchCache: The hosted cache sent an incorrectly formatted response to the client's message to offer it data)",eventId);break;
      case 6404 : snprintf(result,max_size,"%05d (BranchCache: Hosted cache could not be authenticated using the provisioned SSL certificate)",eventId);break;
      case 6405 : snprintf(result,max_size,"%05d (BranchCache: #2 instance(s) of event id #1 occurred)",eventId);break;
      case 6406 : snprintf(result,max_size,"%05d (#1 registered to Windows Firewall to control filtering for the following: )",eventId);break;
      case 6408 : snprintf(result,max_size,"%05d (Registered product #1 failed and Windows Firewall is now controlling the filtering for #2)",eventId);break;
      case 7035 :
      case 7036 : snprintf(result,max_size,"%05d (Status service change)",eventId);break;
      case 8009 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The computer that currently believes it is the master browser is operator2)",eventId);break;
      case 8019 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The browser will continue to attempt to promote itself to the master browser but will no longer log any events in the event log in Event Viewer)",eventId);break;
      case 8020 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The computer that currently believes it is the master browser is unknown)",eventId);break;
      case 8033 : snprintf(result,max_size,"%05d (The browser has forced an election on network /Device/NetBT_E100B5 because a master browser was stopped)",eventId);break;
      case 10005 : snprintf(result,max_size,"%05d (Error)",eventId);break;
      case 11500 : snprintf(result,max_size,"%05d (Error, another installation is in progress)",eventId);break;
      case 11601 : snprintf(result,max_size,"%05d (Error, disk full)",eventId);return TRUE;
      case 11706 : snprintf(result,max_size,"%05d (Installation error)",eventId);break;
      case 11707 : snprintf(result,max_size,"%05d (Installation successfull)",eventId);break;
      case 11708 : snprintf(result,max_size,"%05d (Installation failed)",eventId);break;
      case 11711 : snprintf(result,max_size,"%05d (An error occurred while writing installation information to disk)",eventId);break;
      case 11724 : snprintf(result,max_size,"%05d (Removal completed successfully)",eventId);break;
      case 11728: snprintf(result,max_size,"%05d (Configuration successfull)",eventId);break;
      case 11729: snprintf(result,max_size,"%05d (Configuration Failed)",eventId);break;
      case 15007: snprintf(result,max_size,"%05d (Reservation for namespace identified by URL prefix http://*:2869/ was successfully added)",eventId);break;
      case 60054: snprintf(result,max_size,"%05d (Setup successfully installed Windows build 2600)",eventId);break;
      default : snprintf(result,max_size,"%05d",eventId);break;
    }
    return FALSE;
}
//------------------------------------------------------------------------------
BOOL EventIdtoDscr(unsigned int eventId, char *source, char *result, unsigned short max_size)
{
  result[0]=0;
  if (!strcmp(source,"Security"))
  {
    switch(eventId)
    {
      case 512 : snprintf(result,max_size,"%05d (Windows NT is starting up)",eventId);return TRUE;
      case 513 : snprintf(result,max_size,"%05d (Windows is shutting down)",eventId);return TRUE;
      case 514 : snprintf(result,max_size,"%05d (An authentication package has been loaded by the Local Security Authority)",eventId);break;
      case 515 : snprintf(result,max_size,"%05d (A trusted logon process has registered with the Local Security Authority)",eventId);break;
      case 516 : snprintf(result,max_size,"%05d (Internal resources allocated for the queuing of audit messages have been exhausted, leading to the loss of some audits)",eventId);break;
      case 517 : snprintf(result,max_size,"%05d (The audit log was cleared)",eventId);return TRUE;
      case 518 : snprintf(result,max_size,"%05d (A notification package has been loaded by the Security Account Manager)",eventId);break;
      case 519 : snprintf(result,max_size,"%05d (A process is using an invalid local procedure call (LPC) port)",eventId);break;
      case 520 : snprintf(result,max_size,"%05d (The system time was changed)",eventId);return TRUE;
      case 528 : snprintf(result,max_size,"%05d (Successful Logon)",eventId);return TRUE;
      case 529 : snprintf(result,max_size,"%05d (Logon Failure - Unknown user name or bad password)",eventId);return TRUE;
      case 530 : snprintf(result,max_size,"%05d (Logon Failure - Account logon time restriction violation)",eventId);return TRUE;
      case 531 : snprintf(result,max_size,"%05d (Logon Failure - Account currently disabled)",eventId);return TRUE;
      case 532 : snprintf(result,max_size,"%05d (Logon Failure - The specified user account has expired)",eventId);return TRUE;
      case 533 : snprintf(result,max_size,"%05d (Logon Failure - User not allowed to logon at this computer)",eventId);return TRUE;
      case 534 : snprintf(result,max_size,"%05d (Logon Failure - The user has not been granted the requested logon type at this machine)",eventId);return TRUE;
      case 535 : snprintf(result,max_size,"%05d (Logon Failure - The specified account's password has expired)",eventId);return TRUE;
      case 536 : snprintf(result,max_size,"%05d (Logon Failure - The NetLogon component is not active)",eventId);return TRUE;
      case 537 : snprintf(result,max_size,"%05d (Logon failure - The logon attempt failed for other reasons)",eventId);return TRUE;
      case 538 : snprintf(result,max_size,"%05d (User Logoff)",eventId);return TRUE;
      case 539 : snprintf(result,max_size,"%05d (Logon Failure - Account locked out)",eventId);return TRUE;
      case 540 : snprintf(result,max_size,"%05d (Successful Network Logon)",eventId);return TRUE;
      case 551 : snprintf(result,max_size,"%05d (User initiated logoff)",eventId);return TRUE;
      case 552 : snprintf(result,max_size,"%05d (Logon attempt using explicit credentials)",eventId);return TRUE;
      case 560 : snprintf(result,max_size,"%05d (Object Open)",eventId);break;
      case 561 : snprintf(result,max_size,"%05d (Handle Allocated)",eventId);break;
      case 562 : snprintf(result,max_size,"%05d (Handle Closed)",eventId);break;
      case 563 : snprintf(result,max_size,"%05d (Object Open for Delete)",eventId);break;
      case 564 : snprintf(result,max_size,"%05d (Object Deleted)",eventId);break;
      case 565 : snprintf(result,max_size,"%05d (Object Open (Active Directory))",eventId);break;
      case 566 : snprintf(result,max_size,"%05d (Object Operation (W3 Active Directory))",eventId);break;
      case 567 : snprintf(result,max_size,"%05d (Object Access Attempt)",eventId);break;
      case 576 : snprintf(result,max_size,"%05d (Special privileges assigned to new logon)",eventId);return TRUE;
      case 577 : snprintf(result,max_size,"%05d (Privileged Service Called)",eventId);break;
      case 578 : snprintf(result,max_size,"%05d (Privileged object operation)",eventId);break;
      case 592 : snprintf(result,max_size,"%05d (A new process has been created)",eventId);break;
      case 593 : snprintf(result,max_size,"%05d (A process has exited)",eventId);break;
      case 594 : snprintf(result,max_size,"%05d (A handle to an object has been duplicated)",eventId);break;
      case 595 : snprintf(result,max_size,"%05d (Indirect access to an object has been obtained)",eventId);break;
      case 600 : snprintf(result,max_size,"%05d (A process was assigned a primary token)",eventId);break;
      case 601 : snprintf(result,max_size,"%05d (Attempt to install service)",eventId);break;
      case 602 : snprintf(result,max_size,"%05d (Scheduled Task created)",eventId);break;
      case 608 : snprintf(result,max_size,"%05d (User Right Assigned)",eventId);return TRUE;
      case 609 : snprintf(result,max_size,"%05d (User Right Removed)",eventId);break;
      case 610 : snprintf(result,max_size,"%05d (New Trusted Domain)",eventId);break;
      case 611 : snprintf(result,max_size,"%05d (Removing Trusted Domain)",eventId);break;
      case 612 : snprintf(result,max_size,"%05d (Audit Policy Change)",eventId);return TRUE;
      case 613 : snprintf(result,max_size,"%05d (IPSec policy agent started)",eventId);break;
      case 614 : snprintf(result,max_size,"%05d (IPSec policy agent disabled)",eventId);break;
      case 615 : snprintf(result,max_size,"%05d (IPSEC PolicyAgent Service)",eventId);break;
      case 616 : snprintf(result,max_size,"%05d (IPSec policy agent encountered a potentially serious failure)",eventId);break;
      case 617 : snprintf(result,max_size,"%05d (Kerberos Policy Changed)",eventId);break;
      case 618 : snprintf(result,max_size,"%05d (Encrypted Data Recovery Policy Changed)",eventId);break;
      case 619 : snprintf(result,max_size,"%05d (Quality of Service Policy Changed)",eventId);break;
      case 620 : snprintf(result,max_size,"%05d (Trusted Domain Information Modified)",eventId);break;
      case 621 : snprintf(result,max_size,"%05d (System Security Access Granted)",eventId);break;
      case 622 : snprintf(result,max_size,"%05d (System Security Access Removed)",eventId);break;
      case 623 : snprintf(result,max_size,"%05d (Per User Audit Policy was refreshed)",eventId);break;
      case 624 : snprintf(result,max_size,"%05d (User Account Created)",eventId);return TRUE;
      case 625 : snprintf(result,max_size,"%05d (User Account Type Changed)",eventId);return TRUE;
      case 626 : snprintf(result,max_size,"%05d (User Account Enabled)",eventId);return TRUE;
      case 627 : snprintf(result,max_size,"%05d (Change Password Attempt)",eventId);return TRUE;
      case 628 : snprintf(result,max_size,"%05d (User Account password set)",eventId);return TRUE;
      case 629 : snprintf(result,max_size,"%05d (User Account Disabled)",eventId);return TRUE;
      case 630 : snprintf(result,max_size,"%05d (User Account Deleted)",eventId);return TRUE;
      case 631 : snprintf(result,max_size,"%05d (Security Enabled Global Group Created)",eventId);break;
      case 632 : snprintf(result,max_size,"%05d (Security Enabled Global Group Member Added)",eventId);break;
      case 633 : snprintf(result,max_size,"%05d (Security Enabled Global Group Member Removed)",eventId);break;
      case 634 : snprintf(result,max_size,"%05d (Security Enabled Global Group Deleted)",eventId);break;
      case 635 : snprintf(result,max_size,"%05d (Security Enabled Local Group Created)",eventId);break;
      case 636 : snprintf(result,max_size,"%05d (Security Enabled Local Group Member Added)",eventId);break;
      case 637 : snprintf(result,max_size,"%05d (Security Enabled Local Group Member Removed)",eventId);break;
      case 638 : snprintf(result,max_size,"%05d (Security Enabled Local Group Deleted)",eventId);break;
      case 639 : snprintf(result,max_size,"%05d (Security Enabled Local Group Changed)",eventId);break;
      case 640 : snprintf(result,max_size,"%05d (General Account Database Change)",eventId);break;
      case 641 : snprintf(result,max_size,"%05d (Security Enabled Global Group Changed)",eventId);break;
      case 642 : snprintf(result,max_size,"%05d (User Account Changed)",eventId);return TRUE;
      case 643 : snprintf(result,max_size,"%05d (Domain Policy Changed)",eventId);break;
      case 644 : snprintf(result,max_size,"%05d (User Account Locked Out)",eventId);return TRUE;
      case 645 : snprintf(result,max_size,"%05d (Computer Account Created)",eventId);return TRUE;
      case 646 : snprintf(result,max_size,"%05d (Computer Account Changed)",eventId);return TRUE;
      case 647 : snprintf(result,max_size,"%05d (Computer Account Deleted)",eventId);return TRUE;
      case 648 : snprintf(result,max_size,"%05d (Security Disabled Local Group Created)",eventId);break;
      case 649 : snprintf(result,max_size,"%05d (Security Disabled Local Group Changed)",eventId);break;
      case 650 : snprintf(result,max_size,"%05d (Security Disabled Local Group Member Added)",eventId);break;
      case 651 : snprintf(result,max_size,"%05d (Security Disabled Local Group Member Removed)",eventId);break;
      case 652 : snprintf(result,max_size,"%05d (Security Disabled Local Group Deleted)",eventId);break;
      case 653 : snprintf(result,max_size,"%05d (Security Disabled Global Group Created)",eventId);break;
      case 654 : snprintf(result,max_size,"%05d (Security Disabled Global Group Changed)",eventId);break;
      case 655 : snprintf(result,max_size,"%05d (Security Disabled Global Group Member Added)",eventId);break;
      case 656 : snprintf(result,max_size,"%05d (Security Disabled Global Group Member Removed)",eventId);break;
      case 657 : snprintf(result,max_size,"%05d (Security Disabled Global Group Deleted)",eventId);break;
      case 658 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Created)",eventId);break;
      case 659 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Changed)",eventId);break;
      case 660 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Member Added)",eventId);break;
      case 661 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Member Removed)",eventId);break;
      case 662 : snprintf(result,max_size,"%05d (Security Enabled Universal Group Deleted)",eventId);break;
      case 663 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Created)",eventId);break;
      case 664 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Changed)",eventId);break;
      case 665 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Member Added)",eventId);break;
      case 666 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Member Removed)",eventId);break;
      case 667 : snprintf(result,max_size,"%05d (Security Disabled Universal Group Deleted)",eventId);break;
      case 668 : snprintf(result,max_size,"%05d (Group Type Changed)",eventId);break;
      case 669 : snprintf(result,max_size,"%05d (Add SID History)",eventId);break;
      case 670 : snprintf(result,max_size,"%05d (Add SID History)",eventId);break;
      case 671 : snprintf(result,max_size,"%05d (User Account Unlocked)",eventId);return TRUE;
      case 672 : snprintf(result,max_size,"%05d (Authentication Ticket Granted)",eventId);break;
      case 673 : snprintf(result,max_size,"%05d (Service Ticket Granted)",eventId);break;
      case 674 : snprintf(result,max_size,"%05d (Ticket Granted Renewed)",eventId);break;
      case 675 : snprintf(result,max_size,"%05d (Pre-authentication failed)",eventId);break;
      case 676 : snprintf(result,max_size,"%05d (Authentication Ticket Request Failed)",eventId);break;
      case 677 : snprintf(result,max_size,"%05d (Service Ticket Request Failed)",eventId);break;
      case 678 : snprintf(result,max_size,"%05d (Account Mapped for Logon by)",eventId);break;
      case 679 : snprintf(result,max_size,"%05d (The name: #2 could not be mapped for logon by: #1)",eventId);break;
      case 680 : snprintf(result,max_size,"%05d (Account Used for Logon by)",eventId);return TRUE;
      case 681 : snprintf(result,max_size,"%05d (The logon to account: #2 by: #1 from workstation: #3 failed)",eventId);return TRUE;
      case 682 : snprintf(result,max_size,"%05d (Session reconnected to winstation)",eventId);return TRUE;
      case 683 : snprintf(result,max_size,"%05d (Session disconnected from winstation)",eventId);return TRUE;
      case 684 : snprintf(result,max_size,"%05d (Set ACLs of members in administrators groups)",eventId);break;
      case 685 : snprintf(result,max_size,"%05d (Account Name Changed)",eventId);return TRUE;
      case 686 : snprintf(result,max_size,"%05d (Password of the following user accessed)",eventId);return TRUE;
      case 687 : snprintf(result,max_size,"%05d (Basic Application Group Created)",eventId);break;
      case 688 : snprintf(result,max_size,"%05d (Basic Application Group Changed)",eventId);break;
      case 689 : snprintf(result,max_size,"%05d (Basic Application Group Member Added)",eventId);break;
      case 690 : snprintf(result,max_size,"%05d (Basic Application Group Member Removed)",eventId);break;
      case 691 : snprintf(result,max_size,"%05d (Basic Application Group Non-Member Added)",eventId);break;
      case 692 : snprintf(result,max_size,"%05d (Basic Application Group Non-Member Removed)",eventId);break;
      case 693 : snprintf(result,max_size,"%05d (Basic Application Group Deleted)",eventId);break;
      case 694 : snprintf(result,max_size,"%05d (LDAP Query Group Created)",eventId);break;
      case 695 : snprintf(result,max_size,"%05d (LDAP Query Group Changed)",eventId);break;
      case 696 : snprintf(result,max_size,"%05d (LDAP Query Group Deleted)",eventId);break;
      case 697 : snprintf(result,max_size,"%05d (Password Policy Checking API is called)",eventId);break;
      case 806 : snprintf(result,max_size,"%05d (Per User Audit Policy was refreshed)",eventId);break;
      case 807 : snprintf(result,max_size,"%05d (Per user auditing policy set for user)",eventId);break;
      case 808 : snprintf(result,max_size,"%05d (A security event source has attempted to register)",eventId);break;
      case 809 : snprintf(result,max_size,"%05d (A security event source has attempted to unregister)",eventId);break;
      case 848 : snprintf(result,max_size,"%05d (The following policy was active when the Windows Firewall started)",eventId);break;
      case 849 : snprintf(result,max_size,"%05d (An application was listed as an exception when the Windows Firewall started)",eventId);break;
      case 850 : snprintf(result,max_size,"%05d (A port was listed as an exception when the Windows Firewall started)",eventId);break;
      case 852 : snprintf(result,max_size,"%05d (A change has been made to the Windows Firewall port exception list)",eventId);break;
      case 861 : snprintf(result,max_size,"%05d (The Windows Firewall has detected an application listening for incoming traffic)",eventId);break;
      case 1100 : snprintf(result,max_size,"%05d (The event logging service has shut down)",eventId);return TRUE;
      case 1101 : snprintf(result,max_size,"%05d (Audit events have been dropped by the transport)",eventId);break;
      case 1102 : snprintf(result,max_size,"%05d (The audit log was cleared)",eventId);return TRUE;
      case 1104 : snprintf(result,max_size,"%05d (The security Log is now full)",eventId);return TRUE;
      case 1105 : snprintf(result,max_size,"%05d (Event log automatic backup)",eventId);break;
      case 1108 : snprintf(result,max_size,"%05d (The event logging service encountered an error )",eventId);return TRUE;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Microsoft-Windows-Security-Auditing"))
  {
    switch(eventId)
    {
      case 4500 : snprintf(result,max_size,"%05d (Metabase Add Key)",eventId);break;
      case 4501 : snprintf(result,max_size,"%05d (Metabase Delete Key)",eventId);break;
      case 4502 : snprintf(result,max_size,"%05d (Metabase Delete Chid Keys)",eventId);break;
      case 4503 : snprintf(result,max_size,"%05d (Metabase Copy Key)",eventId);break;
      case 4504 : snprintf(result,max_size,"%05d (Metabase Rename Key)",eventId);break;
      case 4505 : snprintf(result,max_size,"%05d (Metabase Set Data)",eventId);break;
      case 4506 : snprintf(result,max_size,"%05d (Metabase Delete Data)",eventId);break;
      case 4507 : snprintf(result,max_size,"%05d (Metabase Delete All Data)",eventId);break;
      case 4508 : snprintf(result,max_size,"%05d (Metabase Copy Data)",eventId);break;
      case 4509 : snprintf(result,max_size,"%05d (Metabase Set Last Change Time)",eventId);break;
      case 4510 : snprintf(result,max_size,"%05d (Metabase Restore)",eventId);break;
      case 4511 : snprintf(result,max_size,"%05d (Metabase Delete Backup)",eventId);break;
      case 4512 : snprintf(result,max_size,"%05d (Metabase Import)",eventId);break;
      case 4608 : snprintf(result,max_size,"%05d (Windows is starting up)",eventId);return TRUE;
      case 4609 : snprintf(result,max_size,"%05d (Windows is shutting down)",eventId);return TRUE;
      case 4610 : snprintf(result,max_size,"%05d (An authentication package has been loaded by the Local Security Authority)",eventId);break;
      case 4611 : snprintf(result,max_size,"%05d (A trusted logon process has been registered with the Local Security Authority)",eventId);break;
      case 4612 : snprintf(result,max_size,"%05d (Internal resources allocated for the queuing of audit messages have been exhausted, leading to the loss of some audits)",eventId);break;
      case 4614 : snprintf(result,max_size,"%05d (A notification package has been loaded by the Security Account Manager)",eventId);break;
      case 4615 : snprintf(result,max_size,"%05d (Invalid use of LPC port)",eventId);break;
      case 4616 : snprintf(result,max_size,"%05d (The system time was changed)",eventId);return TRUE;
      case 4618 : snprintf(result,max_size,"%05d (A monitored security event pattern has occurred)",eventId);break;
      case 4621 : snprintf(result,max_size,"%05d (Administrator recovered system from CrashOnAuditFail)",eventId);break;
      case 4622 : snprintf(result,max_size,"%05d (A security package has been loaded by the Local Security Authority)",eventId);break;
      case 4624 : snprintf(result,max_size,"%05d (An account was successfully logged on)",eventId);return TRUE;
      case 4625 : snprintf(result,max_size,"%05d (An account failed to log on)",eventId);return TRUE;
      case 4634 : snprintf(result,max_size,"%05d (An account was logged off)",eventId);return TRUE;
      case 4646 : snprintf(result,max_size,"%05d (IKE DoS-prevention mode started)",eventId);break;
      case 4647 : snprintf(result,max_size,"%05d (User initiated logoff)",eventId);break;
      case 4648 : snprintf(result,max_size,"%05d (A logon was attempted using explicit credentials)",eventId);return TRUE;
      case 4649 : snprintf(result,max_size,"%05d (A replay attack was detected)",eventId);break;
      case 4650 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association was established)",eventId);break;
      case 4651 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association was established)",eventId);break;
      case 4652 : snprintf(result,max_size,"%05d (An IPsec Main Mode negotiation failed)",eventId);break;
      case 4653 : snprintf(result,max_size,"%05d (An IPsec Main Mode negotiation failed)",eventId);break;
      case 4654 : snprintf(result,max_size,"%05d (An IPsec Quick Mode negotiation failed)",eventId);break;
      case 4655 : snprintf(result,max_size,"%05d (An IPsec Main Mode security association ended)",eventId);break;
      case 4656 : snprintf(result,max_size,"%05d (A handle to an object was requested)",eventId);break;
      case 4657 : snprintf(result,max_size,"%05d (A registry value was modified)",eventId);break;
      case 4658 : snprintf(result,max_size,"%05d (The handle to an object was closed)",eventId);break;
      case 4659 : snprintf(result,max_size,"%05d (A handle to an object was requested with intent to delete)",eventId);break;
      case 4660 : snprintf(result,max_size,"%05d (An object was deleted)",eventId);break;
      case 4661 : snprintf(result,max_size,"%05d (A handle to an object was requested)",eventId);break;
      case 4662 : snprintf(result,max_size,"%05d (An operation was performed on an object)",eventId);break;
      case 4663 : snprintf(result,max_size,"%05d (An attempt was made to access an object)",eventId);break;
      case 4664 : snprintf(result,max_size,"%05d (An attempt was made to create a hard link)",eventId);break;
      case 4665 : snprintf(result,max_size,"%05d (An attempt was made to create an application client context)",eventId);break;
      case 4666 : snprintf(result,max_size,"%05d (An application attempted an operation)",eventId);break;
      case 4667 : snprintf(result,max_size,"%05d (An application client context was deleted)",eventId);break;
      case 4668 : snprintf(result,max_size,"%05d (An application was initialized)",eventId);break;
      case 4670 : snprintf(result,max_size,"%05d (Permissions on an object were changed)",eventId);break;
      case 4671 : snprintf(result,max_size,"%05d (An application attempted to access a blocked ordinal through the TBS)",eventId);break;
      case 4672 : snprintf(result,max_size,"%05d (Special privileges assigned to new logon)",eventId);break;
      case 4673 : snprintf(result,max_size,"%05d (A privileged service was called)",eventId);break;
      case 4674 : snprintf(result,max_size,"%05d (An operation was attempted on a privileged object)",eventId);break;
      case 4675 : snprintf(result,max_size,"%05d (SIDs were filtered)",eventId);break;
      case 4688 : snprintf(result,max_size,"%05d (A new process has been created)",eventId);break;
      case 4689 : snprintf(result,max_size,"%05d (A process has exited)",eventId);break;
      case 4690 : snprintf(result,max_size,"%05d (An attempt was made to duplicate a handle to an object)",eventId);break;
      case 4691 : snprintf(result,max_size,"%05d (Indirect access to an object was requested)",eventId);break;
      case 4692 : snprintf(result,max_size,"%05d (Backup of data protection master key was attempted)",eventId);break;
      case 4693 : snprintf(result,max_size,"%05d (Recovery of data protection master key was attempted)",eventId);break;
      case 4694 : snprintf(result,max_size,"%05d (Protection of auditable protected data was attempted)",eventId);break;
      case 4695 : snprintf(result,max_size,"%05d (Unprotection of auditable protected data was attempted)",eventId);break;
      case 4696 : snprintf(result,max_size,"%05d (A primary token was assigned to process)",eventId);break;
      case 4697 : snprintf(result,max_size,"%05d (A service was installed in the system)",eventId);break;
      case 4698 : snprintf(result,max_size,"%05d (A scheduled task was created)",eventId);break;
      case 4699 : snprintf(result,max_size,"%05d (A scheduled task was deleted)",eventId);break;
      case 4700 : snprintf(result,max_size,"%05d (A scheduled task was enabled)",eventId);break;
      case 4701 : snprintf(result,max_size,"%05d (A scheduled task was disabled)",eventId);break;
      case 4702 : snprintf(result,max_size,"%05d (A scheduled task was updated)",eventId);break;
      case 4704 : snprintf(result,max_size,"%05d (A user right was assigned)",eventId);break;
      case 4705 : snprintf(result,max_size,"%05d (A user right was removed)",eventId);break;
      case 4706 : snprintf(result,max_size,"%05d (A new trust was created to a domain)",eventId);break;
      case 4707 : snprintf(result,max_size,"%05d (A trust to a domain was removed)",eventId);break;
      case 4709 : snprintf(result,max_size,"%05d (IPsec Services was started)",eventId);break;
      case 4710 : snprintf(result,max_size,"%05d (IPsec Services was disabled)",eventId);break;
      case 4711 : snprintf(result,max_size,"%05d (PAStore Engine (#1))",eventId);break;
      case 4712 : snprintf(result,max_size,"%05d (IPsec Services encountered a potentially serious failure)",eventId);break;
      case 4713 : snprintf(result,max_size,"%05d (Kerberos policy was changed)",eventId);break;
      case 4714 : snprintf(result,max_size,"%05d (Encrypted data recovery policy was changed)",eventId);break;
      case 4715 : snprintf(result,max_size,"%05d (The audit policy (SACL) on an object was changed)",eventId);break;
      case 4716 : snprintf(result,max_size,"%05d (Trusted domain information was modified)",eventId);break;
      case 4717 : snprintf(result,max_size,"%05d (System security access was granted to an account)",eventId);break;
      case 4718 : snprintf(result,max_size,"%05d (System security access was removed from an account)",eventId);break;
      case 4719 : snprintf(result,max_size,"%05d (System audit policy was changed)",eventId);return TRUE;
      case 4720 : snprintf(result,max_size,"%05d (A user account was created)",eventId);return TRUE;
      case 4722 : snprintf(result,max_size,"%05d (A user account was enabled)",eventId);return TRUE;
      case 4723 : snprintf(result,max_size,"%05d (An attempt was made to change an account's password)",eventId);return TRUE;
      case 4724 : snprintf(result,max_size,"%05d (An attempt was made to reset an accounts password)",eventId);return TRUE;
      case 4725 : snprintf(result,max_size,"%05d (A user account was disabled)",eventId);return TRUE;
      case 4726 : snprintf(result,max_size,"%05d (A user account was deleted)",eventId);return TRUE;
      case 4727 : snprintf(result,max_size,"%05d (A security-enabled global group was created)",eventId);break;
      case 4728 : snprintf(result,max_size,"%05d (A member was added to a security-enabled global group)",eventId);break;
      case 4729 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled global group)",eventId);break;
      case 4730 : snprintf(result,max_size,"%05d (A security-enabled global group was deleted)",eventId);break;
      case 4731 : snprintf(result,max_size,"%05d (A security-enabled local group was created)",eventId);break;
      case 4732 : snprintf(result,max_size,"%05d (A member was added to a security-enabled local group)",eventId);break;
      case 4733 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled local group)",eventId);break;
      case 4734 : snprintf(result,max_size,"%05d (A security-enabled local group was deleted)",eventId);break;
      case 4735 : snprintf(result,max_size,"%05d (A security-enabled local group was changed)",eventId);break;
      case 4737 : snprintf(result,max_size,"%05d (A security-enabled global group was changed)",eventId);break;
      case 4738 : snprintf(result,max_size,"%05d (A user account was changed)",eventId);return TRUE;
      case 4739 : snprintf(result,max_size,"%05d (Domain Policy was changed)",eventId);break;
      case 4740 : snprintf(result,max_size,"%05d (A user account was locked out)",eventId);return TRUE;
      case 4741 : snprintf(result,max_size,"%05d (A computer account was created)",eventId);return TRUE;
      case 4742 : snprintf(result,max_size,"%05d (A computer account was changed)",eventId);return TRUE;
      case 4743 : snprintf(result,max_size,"%05d (A computer account was deleted)",eventId);return TRUE;
      case 4744 : snprintf(result,max_size,"%05d (A security-disabled local group was created)",eventId);break;
      case 4745 : snprintf(result,max_size,"%05d (A security-disabled local group was changed)",eventId);break;
      case 4746 : snprintf(result,max_size,"%05d (A member was added to a security-disabled local group)",eventId);break;
      case 4747 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled local group)",eventId);break;
      case 4748 : snprintf(result,max_size,"%05d (A security-disabled local group was deleted)",eventId);break;
      case 4749 : snprintf(result,max_size,"%05d (A security-disabled global group was created)",eventId);break;
      case 4750 : snprintf(result,max_size,"%05d (A security-disabled global group was changed)",eventId);break;
      case 4751 : snprintf(result,max_size,"%05d (A member was added to a security-disabled global group)",eventId);break;
      case 4752 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled global group)",eventId);break;
      case 4753 : snprintf(result,max_size,"%05d (A security-disabled global group was deleted)",eventId);break;
      case 4754 : snprintf(result,max_size,"%05d (A security-enabled universal group was created)",eventId);break;
      case 4755 : snprintf(result,max_size,"%05d (A security-enabled universal group was changed)",eventId);break;
      case 4756 : snprintf(result,max_size,"%05d (A member was added to a security-enabled universal group)",eventId);break;
      case 4757 : snprintf(result,max_size,"%05d (A member was removed from a security-enabled universal group)",eventId);break;
      case 4758 : snprintf(result,max_size,"%05d (A security-enabled universal group was deleted)",eventId);break;
      case 4759 : snprintf(result,max_size,"%05d (A security-disabled universal group was created)",eventId);break;
      case 4760 : snprintf(result,max_size,"%05d (A security-disabled universal group was changed)",eventId);break;
      case 4761 : snprintf(result,max_size,"%05d (A member was added to a security-disabled universal group)",eventId);break;
      case 4762 : snprintf(result,max_size,"%05d (A member was removed from a security-disabled universal group)",eventId);break;
      case 4763 : snprintf(result,max_size,"%05d (A security-disabled universal group was deleted)",eventId);break;
      case 4764 : snprintf(result,max_size,"%05d (A groups type was changed)",eventId);break;
      case 4765 : snprintf(result,max_size,"%05d (SID History was added to an account)",eventId);break;
      case 4766 : snprintf(result,max_size,"%05d (An attempt to add SID History to an account failed)",eventId);break;
      case 4767 : snprintf(result,max_size,"%05d (A user account was unlocked)",eventId);break;
      case 4768 : snprintf(result,max_size,"%05d (A Kerberos authentication ticket (TGT) was requested)",eventId);break;
      case 4769 : snprintf(result,max_size,"%05d (A Kerberos service ticket was requested)",eventId);break;
      case 4770 : snprintf(result,max_size,"%05d (A Kerberos service ticket was renewed)",eventId);break;
      case 4771 : snprintf(result,max_size,"%05d (Kerberos pre-authentication failed)",eventId);break;
      case 4772 : snprintf(result,max_size,"%05d (A Kerberos authentication ticket request failed)",eventId);break;
      case 4773 : snprintf(result,max_size,"%05d (A Kerberos service ticket request failed)",eventId);break;
      case 4774 : snprintf(result,max_size,"%05d (An account was mapped for logon)",eventId);break;
      case 4775 : snprintf(result,max_size,"%05d (An account could not be mapped for logon)",eventId);break;
      case 4776 : snprintf(result,max_size,"%05d (The domain controller attempted to validate the credentials for an account)",eventId);break;
      case 4777 : snprintf(result,max_size,"%05d (The domain controller failed to validate the credentials for an account)",eventId);break;
      case 4778 : snprintf(result,max_size,"%05d (A session was reconnected to a Window Station)",eventId);break;
      case 4779 : snprintf(result,max_size,"%05d (A session was disconnected from a Window Station)",eventId);break;
      case 4780 : snprintf(result,max_size,"%05d (The ACL was set on accounts which are members of administrators groups)",eventId);break;
      case 4781 : snprintf(result,max_size,"%05d (The name of an account was changed)",eventId);break;
      case 4782 : snprintf(result,max_size,"%05d (The password hash an account was accessed)",eventId);break;
      case 4783 : snprintf(result,max_size,"%05d (A basic application group was created)",eventId);break;
      case 4784 : snprintf(result,max_size,"%05d (A basic application group was changed)",eventId);break;
      case 4785 : snprintf(result,max_size,"%05d (A member was added to a basic application group)",eventId);break;
      case 4786 : snprintf(result,max_size,"%05d (A member was removed from a basic application group)",eventId);break;
      case 4787 : snprintf(result,max_size,"%05d (A non-member was added to a basic application group)",eventId);break;
      case 4788 : snprintf(result,max_size,"%05d (A non-member was removed from a basic application group.)",eventId);break;
      case 4789 : snprintf(result,max_size,"%05d (A basic application group was deleted)",eventId);break;
      case 4790 : snprintf(result,max_size,"%05d (An LDAP query group was created)",eventId);break;
      case 4791 : snprintf(result,max_size,"%05d (A basic application group was changed)",eventId);break;
      case 4792 : snprintf(result,max_size,"%05d (An LDAP query group was deleted)",eventId);break;
      case 4793 : snprintf(result,max_size,"%05d (The Password Policy Checking API was called)",eventId);break;
      case 4794 : snprintf(result,max_size,"%05d (An attempt was made to set the Directory Services Restore Mode administrator password)",eventId);break;
      case 4800 : snprintf(result,max_size,"%05d (The workstation was locked)",eventId);return TRUE;
      case 4801 : snprintf(result,max_size,"%05d (The workstation was unlocked)",eventId);return TRUE;
      case 4802 : snprintf(result,max_size,"%05d (The screen saver was invoked)",eventId);break;
      case 4803 : snprintf(result,max_size,"%05d (The screen saver was dismissed)",eventId);break;
      case 4816 : snprintf(result,max_size,"%05d (RPC detected an integrity violation while decrypting an incoming message)",eventId);break;
      case 4817 : snprintf(result,max_size,"%05d (Auditing settings on object were changed)",eventId);break;
      case 4864 : snprintf(result,max_size,"%05d (A namespace collision was detected)",eventId);break;
      case 4865 : snprintf(result,max_size,"%05d (A trusted forest information entry was added)",eventId);break;
      case 4866 : snprintf(result,max_size,"%05d (A trusted forest information entry was removed)",eventId);break;
      case 4867 : snprintf(result,max_size,"%05d (A trusted forest information entry was modified)",eventId);break;
      case 4868 : snprintf(result,max_size,"%05d (The certificate manager denied a pending certificate request)",eventId);break;
      case 4869 : snprintf(result,max_size,"%05d (Certificate Services received a resubmitted certificate request)",eventId);break;
      case 4870 : snprintf(result,max_size,"%05d (Certificate Services revoked a certificate)",eventId);break;
      case 4871 : snprintf(result,max_size,"%05d (Certificate Services received a request to publish the certificate revocation list (CRL))",eventId);break;
      case 4872 : snprintf(result,max_size,"%05d (Certificate Services published the certificate revocation list (CRL))",eventId);break;
      case 4873 : snprintf(result,max_size,"%05d (A certificate request extension changed)",eventId);break;
      case 4874 : snprintf(result,max_size,"%05d (One or more certificate request attributes changed)",eventId);break;
      case 4875 : snprintf(result,max_size,"%05d (Certificate Services received a request to shut down)",eventId);break;
      case 4876 : snprintf(result,max_size,"%05d (Certificate Services backup started)",eventId);break;
      case 4877 : snprintf(result,max_size,"%05d (Certificate Services backup completed)",eventId);break;
      case 4878 : snprintf(result,max_size,"%05d (Certificate Services restore started)",eventId);break;
      case 4879 : snprintf(result,max_size,"%05d (Certificate Services restore completed)",eventId);break;
      case 4880 : snprintf(result,max_size,"%05d (Certificate Services started)",eventId);break;
      case 4881 : snprintf(result,max_size,"%05d (Certificate Services stopped)",eventId);break;
      case 4882 : snprintf(result,max_size,"%05d (The security permissions for Certificate Services changed)",eventId);break;
      case 4883 : snprintf(result,max_size,"%05d (Certificate Services retrieved an archived key)",eventId);break;
      case 4884 : snprintf(result,max_size,"%05d (Certificate Services imported a certificate into its database)",eventId);break;
      case 4885 : snprintf(result,max_size,"%05d (The audit filter for Certificate Services changed)",eventId);break;
      case 4886 : snprintf(result,max_size,"%05d (Certificate Services received a certificate request)",eventId);break;
      case 4887 : snprintf(result,max_size,"%05d (Certificate Services approved a certificate request and issued a certificate)",eventId);break;
      case 4888 : snprintf(result,max_size,"%05d (Certificate Services denied a certificate request)",eventId);break;
      case 4889 : snprintf(result,max_size,"%05d (Certificate Services set the status of a certificate request to pending)",eventId);break;
      case 4890 : snprintf(result,max_size,"%05d (The certificate manager settings for Certificate Services changed)",eventId);break;
      case 4891 : snprintf(result,max_size,"%05d (A configuration entry changed in Certificate Services)",eventId);break;
      case 4892 : snprintf(result,max_size,"%05d (A property of Certificate Services changed)",eventId);break;
      case 4893 : snprintf(result,max_size,"%05d (Certificate Services archived a key)",eventId);break;
      case 4894 : snprintf(result,max_size,"%05d (Certificate Services imported and archived a key)",eventId);break;
      case 4895 : snprintf(result,max_size,"%05d (Certificate Services published the CA certificate to Active Directory Domain Services)",eventId);break;
      case 4896 : snprintf(result,max_size,"%05d (One or more rows have been deleted from the certificate database)",eventId);break;
      case 4897 : snprintf(result,max_size,"%05d (Role separation enabled)",eventId);break;
      case 4898 : snprintf(result,max_size,"%05d (Certificate Services loaded a template)",eventId);break;
      case 4899 : snprintf(result,max_size,"%05d (A Certificate Services template was updated)",eventId);break;
      case 4900 : snprintf(result,max_size,"%05d (Certificate Services template security was updated)",eventId);break;
      case 4902 : snprintf(result,max_size,"%05d (The Per-user audit policy table was created)",eventId);break;
      case 4904 : snprintf(result,max_size,"%05d (An attempt was made to register a security event source)",eventId);break;
      case 4905 : snprintf(result,max_size,"%05d (An attempt was made to unregister a security event source)",eventId);break;
      case 4906 : snprintf(result,max_size,"%05d (The CrashOnAuditFail value has changed)",eventId);break;
      case 4907 : snprintf(result,max_size,"%05d (Auditing settings on object were changed)",eventId);break;
      case 4908 : snprintf(result,max_size,"%05d (Special Groups Logon table modified)",eventId);break;
      case 4909 : snprintf(result,max_size,"%05d (The local policy settings for the TBS were changed)",eventId);break;
      case 4910 : snprintf(result,max_size,"%05d (The group policy settings for the TBS were changed)",eventId);break;
      case 4912 : snprintf(result,max_size,"%05d (Per User Audit Policy was changed)",eventId);break;
      case 4928 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was established)",eventId);break;
      case 4929 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was removed)",eventId);break;
      case 4930 : snprintf(result,max_size,"%05d (An Active Directory replica source naming context was modified)",eventId);break;
      case 4931 : snprintf(result,max_size,"%05d (An Active Directory replica destination naming context was modified)",eventId);break;
      case 4932 : snprintf(result,max_size,"%05d (Synchronization of a replica of an Active Directory naming context has begun)",eventId);break;
      case 4933 : snprintf(result,max_size,"%05d (Synchronization of a replica of an Active Directory naming context has ended)",eventId);break;
      case 4934 : snprintf(result,max_size,"%05d (Attributes of an Active Directory object were replicated)",eventId);break;
      case 4935 : snprintf(result,max_size,"%05d (Replication failure begins)",eventId);break;
      case 4936 : snprintf(result,max_size,"%05d (Replication failure ends)",eventId);break;
      case 4937 : snprintf(result,max_size,"%05d (A lingering object was removed from a replica)",eventId);break;
      case 4944 : snprintf(result,max_size,"%05d (The following policy was active when the Windows Firewall started)",eventId);break;
      case 4945 : snprintf(result,max_size,"%05d (A rule was listed when the Windows Firewall started)",eventId);break;
      case 4946 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was added)",eventId);break;
      case 4947 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was modified)",eventId);break;
      case 4948 : snprintf(result,max_size,"%05d (A change has been made to Windows Firewall exception list. A rule was deleted)",eventId);break;
      case 4949 : snprintf(result,max_size,"%05d (Windows Firewall settings were restored to the default values)",eventId);break;
      case 4950 : snprintf(result,max_size,"%05d (A Windows Firewall setting has changed)",eventId);break;
      case 4951 : snprintf(result,max_size,"%05d (A rule has been ignored because its major version number was not recognized by Windows Firewall)",eventId);break;
      case 4952 : snprintf(result,max_size,"%05d (Parts of a rule have been ignored because its minor version number was not recognized by Windows Firewall)",eventId);break;
      case 4953 : snprintf(result,max_size,"%05d (A rule has been ignored by Windows Firewall because it could not parse the rule)",eventId);break;
      case 4954 : snprintf(result,max_size,"%05d (Windows Firewall Group Policy settings has changed. The new settings have been applied)",eventId);break;
      case 4956 : snprintf(result,max_size,"%05d (Windows Firewall has changed the active profile)",eventId);break;
      case 4957 : snprintf(result,max_size,"%05d (Windows Firewall did not apply the following rule)",eventId);break;
      case 4958 : snprintf(result,max_size,"%05d (Windows Firewall did not apply the following rule because the rule referred to items not configured on this computer)",eventId);break;
      case 4960 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed an integrity check)",eventId);break;
      case 4961 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed a replay check)",eventId);break;
      case 4962 : snprintf(result,max_size,"%05d (IPsec dropped an inbound packet that failed a replay check)",eventId);break;
      case 4963 : snprintf(result,max_size,"%05d (IPsec dropped an inbound clear text packet that should have been secured)",eventId);break;
      case 4964 : snprintf(result,max_size,"%05d (Special groups have been assigned to a new logon)",eventId);break;
      case 4965 : snprintf(result,max_size,"%05d (IPsec received a packet from a remote computer with an incorrect Security Parameter Index (SPI))",eventId);break;
      case 4976 : snprintf(result,max_size,"%05d (During Main Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4977 : snprintf(result,max_size,"%05d (During Quick Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4978 : snprintf(result,max_size,"%05d (During Extended Mode negotiation, IPsec received an invalid negotiation packet)",eventId);break;
      case 4979 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4980 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4981 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4982 : snprintf(result,max_size,"%05d (IPsec Main Mode and Extended Mode security associations were established)",eventId);break;
      case 4983 : snprintf(result,max_size,"%05d (An IPsec Extended Mode negotiation failed)",eventId);break;
      case 4984 : snprintf(result,max_size,"%05d (An IPsec Extended Mode negotiation failed)",eventId);break;
      case 4985 : snprintf(result,max_size,"%05d (The state of a transaction has changed)",eventId);break;
      case 5024 : snprintf(result,max_size,"%05d (The Windows Firewall Service has started successfully)",eventId);break;
      case 5025 : snprintf(result,max_size,"%05d (The Windows Firewall Service has been stopped)",eventId);break;
      case 5027 : snprintf(result,max_size,"%05d (The Windows Firewall Service was unable to retrieve the security policy from the local storage)",eventId);break;
      case 5028 : snprintf(result,max_size,"%05d (The Windows Firewall Service was unable to parse the new security policy)",eventId);break;
      case 5029 : snprintf(result,max_size,"%05d (The Windows Firewall Service failed to initialize the driver)",eventId);break;
      case 5030 : snprintf(result,max_size,"%05d (The Windows Firewall Service failed to start)",eventId);break;
      case 5031 : snprintf(result,max_size,"%05d (The Windows Firewall Service blocked an application from accepting incoming connections on the network)",eventId);break;
      case 5032 : snprintf(result,max_size,"%05d (Windows Firewall was unable to notify the user that it blocked an application from accepting incoming connections on the network)",eventId);break;
      case 5033 : snprintf(result,max_size,"%05d (The Windows Firewall Driver has started successfully)",eventId);break;
      case 5034 : snprintf(result,max_size,"%05d (The Windows Firewall Driver has been stopped)",eventId);break;
      case 5035 : snprintf(result,max_size,"%05d (The Windows Firewall Driver failed to start)",eventId);break;
      case 5037 : snprintf(result,max_size,"%05d (The Windows Firewall Driver detected critical runtime error. Terminating)",eventId);break;
      case 5038 : snprintf(result,max_size,"%05d (Code integrity determined that the image hash of a file is not valid)",eventId);break;
      case 5039 : snprintf(result,max_size,"%05d (A registry key was virtualized)",eventId);break;
      case 5040 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was added)",eventId);break;
      case 5041 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was modified)",eventId);break;
      case 5042 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. An Authentication Set was deleted)",eventId);break;
      case 5043 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was added)",eventId);break;
      case 5044 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was modified)",eventId);break;
      case 5045 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Connection Security Rule was deleted)",eventId);break;
      case 5046 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was added)",eventId);break;
      case 5047 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was modified)",eventId);break;
      case 5048 : snprintf(result,max_size,"%05d (A change has been made to IPsec settings. A Crypto Set was deleted)",eventId);break;
      case 5049 : snprintf(result,max_size,"%05d (An IPsec Security Association was deleted)",eventId);break;
      case 5050 : snprintf(result,max_size,"%05d (An attempt to programmatically disable the Windows Firewall using a call to INetFwProfile.FirewallEnabled(FALSE)",eventId);break;
      case 5051 : snprintf(result,max_size,"%05d (A file was virtualized)",eventId);break;
      case 5056 : snprintf(result,max_size,"%05d (A cryptographic self test was performed)",eventId);break;
      case 5057 : snprintf(result,max_size,"%05d (A cryptographic primitive operation failed)",eventId);break;
      case 5058 : snprintf(result,max_size,"%05d (Key file operation)",eventId);break;
      case 5059 : snprintf(result,max_size,"%05d (Key migration operation)",eventId);break;
      case 5060 : snprintf(result,max_size,"%05d (Verification operation failed)",eventId);break;
      case 5061 : snprintf(result,max_size,"%05d (Cryptographic operation)",eventId);break;
      case 5062 : snprintf(result,max_size,"%05d (A kernel-mode cryptographic self test was performed)",eventId);break;
      case 5063 : snprintf(result,max_size,"%05d (A cryptographic provider operation was attempted)",eventId);break;
      case 5064 : snprintf(result,max_size,"%05d (A cryptographic context operation was attempted)",eventId);break;
      case 5065 : snprintf(result,max_size,"%05d (A cryptographic context modification was attempted)",eventId);break;
      case 5066 : snprintf(result,max_size,"%05d (A cryptographic function operation was attempted)",eventId);break;
      case 5067 : snprintf(result,max_size,"%05d (A cryptographic function modification was attempted)",eventId);break;
      case 5068 : snprintf(result,max_size,"%05d (A cryptographic function provider operation was attempted)",eventId);break;
      case 5069 : snprintf(result,max_size,"%05d (A cryptographic function property operation was attempted)",eventId);break;
      case 5070 : snprintf(result,max_size,"%05d (A cryptographic function property operation was attempted)",eventId);break;
      case 5120 : snprintf(result,max_size,"%05d (OCSP Responder Service Started)",eventId);break;
      case 5121 : snprintf(result,max_size,"%05d (OCSP Responder Service Stopped)",eventId);break;
      case 5122 : snprintf(result,max_size,"%05d (A Configuration entry changed in the OCSP Responder Service)",eventId);break;
      case 5123 : snprintf(result,max_size,"%05d (A configuration entry changed in the OCSP Responder Service)",eventId);break;
      case 5124 : snprintf(result,max_size,"%05d (A security setting was updated on OCSP Responder Service)",eventId);break;
      case 5125 : snprintf(result,max_size,"%05d (A request was submitted to OCSP Responder Service)",eventId);break;
      case 5126 : snprintf(result,max_size,"%05d (Signing Certificate was automatically updated by the OCSP Responder Service)",eventId);break;
      case 5127 : snprintf(result,max_size,"%05d (The OCSP Revocation Provider successfully updated the revocation information)",eventId);break;
      case 5136 : snprintf(result,max_size,"%05d (A directory service object was modified)",eventId);break;
      case 5137 : snprintf(result,max_size,"%05d (A directory service object was created)",eventId);break;
      case 5138 : snprintf(result,max_size,"%05d (A directory service object was undeleted)",eventId);break;
      case 5139 : snprintf(result,max_size,"%05d (A directory service object was moved)",eventId);break;
      case 5140 : snprintf(result,max_size,"%05d (A network share object was accessed)",eventId);break;
      case 5141 : snprintf(result,max_size,"%05d (A directory service object was deleted)",eventId);break;
      case 5142 : snprintf(result,max_size,"%05d (A network share object was added)",eventId);break;
      case 5143 : snprintf(result,max_size,"%05d (A network share object was modified)",eventId);break;
      case 5144 : snprintf(result,max_size,"%05d (A network share object was deleted)",eventId);break;
      case 5145 : snprintf(result,max_size,"%05d (A network share object was checked to see whether client can be granted desired access)",eventId);break;
      case 5148 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has detected a DoS attack and entered a defensive mode; packets associated with this attack will be discarded)",eventId);break;
      case 5149 : snprintf(result,max_size,"%05d (The DoS attack has subsided and normal processing is being resumed)",eventId);break;
      case 5150 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a packet)",eventId);break;
      case 5151 : snprintf(result,max_size,"%05d (A more restrictive Windows Filtering Platform filter has blocked a packet)",eventId);break;
      case 5152 : snprintf(result,max_size,"%05d (The Windows Filtering Platform blocked a packet)",eventId);break;
      case 5153 : snprintf(result,max_size,"%05d (A more restrictive Windows Filtering Platform filter has blocked a packet)",eventId);break;
      case 5154 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has permitted an application or service to listen on a port for incoming connections)",eventId);break;
      case 5155 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked an application or service from listening on a port for incoming connections)",eventId);break;
      case 5156 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has allowed a connection)",eventId);break;
      case 5157 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a connection)",eventId);break;
      case 5158 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has permitted a bind to a local port)",eventId);break;
      case 5159 : snprintf(result,max_size,"%05d (The Windows Filtering Platform has blocked a bind to a local port)",eventId);break;
      case 5168 : snprintf(result,max_size,"%05d (Spn check for SMB/SMB2 fails)",eventId);break;
      case 5376 : snprintf(result,max_size,"%05d (Credential Manager credentials were backed up)",eventId);break;
      case 5377 : snprintf(result,max_size,"%05d (Credential Manager credentials were restored from a backup)",eventId);break;
      case 5378 : snprintf(result,max_size,"%05d (The requested credentials delegation was disallowed by policy)",eventId);break;
      case 5440 : snprintf(result,max_size,"%05d (The following callout was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5441 : snprintf(result,max_size,"%05d (The following filter was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5442 : snprintf(result,max_size,"%05d (The following provider was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5443 : snprintf(result,max_size,"%05d (The following provider context was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5444 : snprintf(result,max_size,"%05d (The following sub-layer was present when the Windows Filtering Platform Base Filtering Engine started)",eventId);break;
      case 5446 : snprintf(result,max_size,"%05d (A Windows Filtering Platform callout has been changed)",eventId);break;
      case 5447 : snprintf(result,max_size,"%05d (A Windows Filtering Platform filter has been changed)",eventId);break;
      case 5448 : snprintf(result,max_size,"%05d (A Windows Filtering Platform provider has been changed)",eventId);break;
      case 5449 : snprintf(result,max_size,"%05d (A Windows Filtering Platform provider context has been changed)",eventId);break;
      case 5450 : snprintf(result,max_size,"%05d (A Windows Filtering Platform sub-layer has been changed)",eventId);break;
      case 5451 : snprintf(result,max_size,"%05d (An IPsec Quick Mode security association was established)",eventId);break;
      case 5452 : snprintf(result,max_size,"%05d (An IPsec Quick Mode security association ended)",eventId);break;
      case 5453 : snprintf(result,max_size,"%05d (An IPsec negotiation with a remote computer failed because the IKE and AuthIP IPsec Keying Modules (IKEEXT) service is not started)",eventId);break;
      case 5456 : snprintf(result,max_size,"%05d (PAStore Engine applied Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5457 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5458 : snprintf(result,max_size,"%05d (PAStore Engine applied locally cached copy of Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5459 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply locally cached copy of Active Directory storage IPsec policy on the computer)",eventId);break;
      case 5460 : snprintf(result,max_size,"%05d (PAStore Engine applied local registry storage IPsec policy on the computer)",eventId);break;
      case 5461 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply local registry storage IPsec policy on the computer)",eventId);break;
      case 5462 : snprintf(result,max_size,"%05d (PAStore Engine failed to apply some rules of the active IPsec policy on the computer)",eventId);break;
      case 5463 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the active IPsec policy and detected no changes)",eventId);break;
      case 5464 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the active IPsec policy, detected changes, and applied them to IPsec Services)",eventId);break;
      case 5465 : snprintf(result,max_size,"%05d (PAStore Engine received a control for forced reloading of IPsec policy and processed the control successfully)",eventId);break;
      case 5466 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory cannot be reached, and will use the cached copy of the Active Directory IPsec policy instead)",eventId);break;
      case 5467 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory can be reached, and found no changes to the policy)",eventId);break;
      case 5468 : snprintf(result,max_size,"%05d (PAStore Engine polled for changes to the Active Directory IPsec policy, determined that Active Directory can be reached, found changes to the policy, and applied those changes)",eventId);break;
      case 5471 : snprintf(result,max_size,"%05d (PAStore Engine loaded local storage IPsec policy on the computer)",eventId);break;
      case 5472 : snprintf(result,max_size,"%05d (PAStore Engine failed to load local storage IPsec policy on the computer)",eventId);break;
      case 5473 : snprintf(result,max_size,"%05d (PAStore Engine loaded directory storage IPsec policy on the computer)",eventId);break;
      case 5474 : snprintf(result,max_size,"%05d (PAStore Engine failed to load directory storage IPsec policy on the computer)",eventId);break;
      case 5477 : snprintf(result,max_size,"%05d (PAStore Engine failed to add quick mode filter)",eventId);break;
      case 5478 : snprintf(result,max_size,"%05d (IPsec Services has started successfully)",eventId);break;
      case 5479 : snprintf(result,max_size,"%05d (IPsec Services has been shut down successfully)",eventId);break;
      case 5480 : snprintf(result,max_size,"%05d (IPsec Services failed to get the complete list of network interfaces on the computer)",eventId);break;
      case 5483 : snprintf(result,max_size,"%05d (IPsec Services failed to initialize RPC server. IPsec Services could not be started)",eventId);break;
      case 5484 : snprintf(result,max_size,"%05d (IPsec Services has experienced a critical failure and has been shut down)",eventId);break;
      case 5485 : snprintf(result,max_size,"%05d (IPsec Services failed to process some IPsec filters on a plug-and-play event for network interfaces)",eventId);break;
      case 5632 : snprintf(result,max_size,"%05d (A request was made to authenticate to a wireless network)",eventId);break;
      case 5633 : snprintf(result,max_size,"%05d (A request was made to authenticate to a wired network)",eventId);break;
      case 5712 : snprintf(result,max_size,"%05d (A Remote Procedure Call (RPC) was attempted)",eventId);break;
      case 5888 : snprintf(result,max_size,"%05d (An object in the COM+ Catalog was modified)",eventId);break;
      case 5889 : snprintf(result,max_size,"%05d (An object was deleted from the COM+ Catalog)",eventId);break;
      case 5890 : snprintf(result,max_size,"%05d (An object was added to the COM+ Catalog)",eventId);break;
      case 6144 : snprintf(result,max_size,"%05d (Security policy in the group policy objects has been applied successfully)",eventId);break;
      case 6145 : snprintf(result,max_size,"%05d (One or more errors occured while processing security policy in the group policy objects)",eventId);break;
      case 6272 : snprintf(result,max_size,"%05d (Network Policy Server granted access to a user)",eventId);break;
      case 6273 : snprintf(result,max_size,"%05d (Network Policy Server denied access to a user)",eventId);break;
      case 6274 : snprintf(result,max_size,"%05d (Network Policy Server discarded the request for a user)",eventId);break;
      case 6275 : snprintf(result,max_size,"%05d (Network Policy Server discarded the accounting request for a user)",eventId);break;
      case 6276 : snprintf(result,max_size,"%05d (Network Policy Server quarantined a user)",eventId);break;
      case 6277 : snprintf(result,max_size,"%05d (Network Policy Server granted access to a user but put it on probation because the host did not meet the defined health policy)",eventId);break;
      case 6278 : snprintf(result,max_size,"%05d (Network Policy Server granted full access to a user because the host met the defined health policy)",eventId);break;
      case 6279 : snprintf(result,max_size,"%05d (Network Policy Server locked the user account due to repeated failed authentication attempts)",eventId);break;
      case 6280 : snprintf(result,max_size,"%05d (Network Policy Server unlocked the user account)",eventId);break;
      case 6281 : snprintf(result,max_size,"%05d (Code Integrity determined that the page hashes of an image file are not valid... )",eventId);break;
      case 6400 : snprintf(result,max_size,"%05d (BranchCache: Received an incorrectly formatted response while discovering availability of content. )",eventId);break;
      case 6401 : snprintf(result,max_size,"%05d (BranchCache: Received invalid data from a peer. Data discarded. )",eventId);break;
      case 6402 : snprintf(result,max_size,"%05d (BranchCache: The message to the hosted cache offering it data is incorrectly formatted)",eventId);break;
      case 6403 : snprintf(result,max_size,"%05d (BranchCache: The hosted cache sent an incorrectly formatted response to the client's message to offer it data)",eventId);break;
      case 6404 : snprintf(result,max_size,"%05d (BranchCache: Hosted cache could not be authenticated using the provisioned SSL certificate)",eventId);break;
      case 6405 : snprintf(result,max_size,"%05d (BranchCache: #2 instance(s) of event id #1 occurred)",eventId);break;
      case 6406 : snprintf(result,max_size,"%05d (#1 registered to Windows Firewall to control filtering for the following: )",eventId);break;
      case 6408 : snprintf(result,max_size,"%05d (Registered product #1 failed and Windows Firewall is now controlling the filtering for #2)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Tcpip"))
  {
    switch(eventId)
    {
      case 4201 : snprintf(result,max_size,"%05d (New network connection)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"W32Time") || !strcmp(source,"Microsoft-Windows-Time-Service"))
  {
    switch(eventId)
    {
      case 35 : snprintf(result,max_size,"%05d (Time service synchronised)",eventId);return TRUE;
      case 36 : snprintf(result,max_size,"%05d (Time service not synchronised)",eventId);return TRUE;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Service Control Manager"))
  {
    switch(eventId)
    {
      case 7035 :
      case 7036 : snprintf(result,max_size,"%05d (Status service change)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"EventLog"))
  {
    switch(eventId)
    {
      case 6005 : snprintf(result,max_size,"%05d (Start Event service)",eventId);return TRUE;
      case 6006 : snprintf(result,max_size,"%05d (Stop Event service)",eventId);return TRUE;
      case 6009 : snprintf(result,max_size,"%05d (Description)",eventId);break;
      case 6011 : snprintf(result,max_size,"%05d (New computer name #1 to #2)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"EvntAgnt"))
  {
    switch(eventId)
    {
      case 1003 : snprintf(result,max_size,"%05d (TraceFileName parameter not located in registry)",eventId);break;
      case 1015 : snprintf(result,max_size,"%05d (TraceLevel parameter not located in registry; Default trace level used is 32)",eventId);break;
      case 3006 : snprintf(result,max_size,"%05d (Error reading log event record. Handle specified is 619064. Return code from ReadEventLog is)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"NetBT"))
  {
    switch(eventId)
    {
      case 4321 : snprintf(result,max_size,"%05d (Name registry of ip#1 to ip#2 fail)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"BROWSER"))
  {
    switch(eventId)
    {
      case 8009 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The computer that currently believes it is the master browser is operator2)",eventId);break;
      case 8019 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The browser will continue to attempt to promote itself to the master browser but will no longer log any events in the event log in Event Viewer)",eventId);break;
      case 8020 : snprintf(result,max_size,"%05d (The browser was unable to promote itself to master browser. The computer that currently believes it is the master browser is unknown)",eventId);break;
      case 8033 : snprintf(result,max_size,"%05d (The browser has forced an election on network /Device/NetBT_E100B5 because a master browser was stopped)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Dhcp"))
  {
    switch(eventId)
    {
      case 1000 : snprintf(result,max_size,"%05d (New IP address)",eventId);return TRUE;
      case 1003 : snprintf(result,max_size,"%05d (Error to renew address)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"SRService"))
  {
    switch(eventId)
    {
      case 115 : snprintf(result,max_size,"%05d (System Restore monitoring enabled on all drives)",eventId);break;
      case 116: snprintf(result,max_size,"%05d (System Restore monitoring disable on all drives)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Setup"))
  {
    switch(eventId)
    {
      case 60054: snprintf(result,max_size,"%05d (Setup successfully installed Windows build 2600)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"HTTP"))
  {
    switch(eventId)
    {
      case 15007: snprintf(result,max_size,"%05d (Reservation for namespace identified by URL prefix http://*:2869/ was successfully added)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Workstation"))
  {
    switch(eventId)
    {
      case 3260 : snprintf(result,max_size,"%05d (This computer has been successfully joined domain)",eventId);return TRUE;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"DrWatson"))
  {
    switch(eventId)
    {
      case 4097 : snprintf(result,max_size,"%05d (Error exception)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"HotFixInstaller"))
  {
    switch(eventId)
    {
      case 5000 : snprintf(result,max_size,"%05d (Installation error)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"LoadPerf"))
  {
    switch(eventId)
    {
      case 1000 : snprintf(result,max_size,"%05d (Performance counters for the service installed successfully)",eventId);break;
      case 1001 : snprintf(result,max_size,"%05d (Performance counters for the service were removed successfully)",eventId);break;
      case 1002 : snprintf(result,max_size,"%05d (Performance counters for the service are already installed)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"MSDTC"))
  {
    switch(eventId)
    {
      case 2444 : snprintf(result,max_size,"%05d (MS DTC started)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"MsiInstaller"))
  {
    switch(eventId)
    {
      case 1001 : snprintf(result,max_size,"%05d (Failed during request for component)",eventId);break;
      case 1004 : snprintf(result,max_size,"%05d (Installation failed)",eventId);break;
      case 1005 : snprintf(result,max_size,"%05d (Install operation initiated a reboot)",eventId);break;
      case 1022 : snprintf(result,max_size,"%05d (Update installed successfully)",eventId);break;
      case 1023 : snprintf(result,max_size,"%05d (Update could not be installed)",eventId);break;
      case 10005 : snprintf(result,max_size,"%05d (Error)",eventId);break;
      case 11500 : snprintf(result,max_size,"%05d (Error, another installation is in progress)",eventId);break;
      case 11601 : snprintf(result,max_size,"%05d (Error, disk full)",eventId);return TRUE;
      case 11706 : snprintf(result,max_size,"%05d (Installation error)",eventId);break;
      case 11707 : snprintf(result,max_size,"%05d (Installation successfull)",eventId);break;
      case 11708 : snprintf(result,max_size,"%05d (Installation failed)",eventId);break;
      case 11711 : snprintf(result,max_size,"%05d (An error occurred while writing installation information to disk)",eventId);break;
      case 11724 : snprintf(result,max_size,"%05d (Removal completed successfully)",eventId);break;
      case 11728: snprintf(result,max_size,"%05d (Configuration successfull)",eventId);break;
      case 11729: snprintf(result,max_size,"%05d (Configuration Failed)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"Userenv"))
  {
    switch(eventId)
    {
      case 1517 : snprintf(result,max_size,"%05d (Windows saved user registry while an application or service was still using the registry during log off. The memory used by the user's registry has not been freed)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"VSSetup"))
  {
    switch(eventId)
    {
      case 5000 : snprintf(result,max_size,"%05d (Installation error)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else if (!strcmp(source,"crypt32"))
  {
    switch(eventId)
    {
      case 7 : snprintf(result,max_size,"%05d (Successful auto update retrieval of third-party root list sequence number from :)",eventId);break;
      default : DefaultEventIdtoDscr(eventId, result, max_size);break;
    }
  }else //cas d'id uniques ou erreurs
  {
    return DefaultEventIdtoDscr(eventId, result, max_size);
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//test pour l'import si un journal est critical
BOOL logIsCritcal(char *id, char *src)
{
  if (strlen(id) > 5)
  {

     if (!strcmp(src,"src") ||
        !strcmp(src,"Dhcp") ||
        !strcmp(src,"EventLog") ||
        !strcmp(src,"W32Time") ||
        !strcmp(src,"Microsoft-Windows-Time-Service") ||
        !strcmp(src,"Microsoft-Windows-Security-Auditing") ||
        !strcmp(src,"Security"))
    {
      char id_s[6];
      id_s[0] = id[0];
      id_s[1] = id[1];
      id_s[2] = id[2];
      id_s[3] = id[3];
      id_s[4] = id[4];
      id_s[5] = 0;

      switch(atoi(id_s))
      {
        case 35 :
        case 36 :
        case 512 :
        case 513 :
        case 517 :
        case 520 :
        case 528 :
        case 529 :
        case 530 :
        case 531 :
        case 532 :
        case 533 :
        case 534 :
        case 535 :
        case 536 :
        case 537 :
        case 538 :
        case 539 :
        case 540 :
        case 551 :
        case 552 :
        case 576 :
        case 608 :
        case 612 :
        case 624 :
        case 625 :
        case 626 :
        case 627 :
        case 628 :
        case 629 :
        case 630 :
        case 642 :
        case 644 :
        case 645 :
        case 646 :
        case 647 :
        case 671 :
        case 680 :
        case 681 :
        case 682 :
        case 683 :
        case 685 :
        case 686 :
        case 1000 :
        case 1100 :
        case 1102 :
        case 1104 :
        case 1108 :
        case 3260 :
        case 4608 :
        case 4609 :
        case 4616 :
        case 4624 :
        case 4625 :
        case 4634 :
        case 4648 :
        case 4719 :
        case 4720 :
        case 4722 :
        case 4723 :
        case 4724 :
        case 4725 :
        case 4726 :
        case 4738 :
        case 4740 :
        case 4741 :
        case 4742 :
        case 4743 :
        case 4800 :
        case 4801 :
        case 6005 :
        case 6006 :return TRUE;
      }



    }
  }
  return FALSE;
}
//------------------------------------------------------------------------------
//traitement des chaines de description
char *TraiterDescriptionLog(char *buffer,unsigned short taille, char *resultat, unsigned short taille_resultat)
{
  unsigned short i=0,k=0,j=0;
  BOOL last = FALSE; //permet de vérifier que l'on ajoute pas de chaîne vide ^^

  //traitement esclusivement des caractères standards
  for (;i<taille && k<taille_resultat-1;i++)
  {
    if ((buffer[i]<141 && buffer[i]> 31 && buffer[i]!=127) ||
        buffer[i]=='é' || buffer[i]=='è' || buffer[i]=='ê' ||
        buffer[i]=='ë' || buffer[i]=='à' || buffer[i]=='â' ||
        buffer[i]=='î' || buffer[i]=='ï' || buffer[i]=='ô')
    {
      resultat[k++]=buffer[i];
      j++;
      last = TRUE;
    }else
    {
      if (last)
      {
        if (j<2)//unicode mode ^^
        {
          j=0;
        }else
        {
          resultat[k++]=' ';
          j=0;
        }
        last = FALSE;
      }
    }
  }
  resultat[k]=' ';
  return resultat;
}
//------------------------------------------------------------------------------
//lecture des journaux des derniers aux nouveaux
void LireEvent(HANDLE Heventlog, char *eventname, HANDLE hlv, DWORD cRecords)
{
  EVENTLOGRECORD *pevlr;
  DWORD cbBuffer = 1024*64;
  BYTE bBuffer[(1024*64)+1];
  pevlr = (EVENTLOGRECORD *) &bBuffer;
  DWORD dwRead, dwNeeded;
  PSID lpSid;
  char szName[MAX_PATH], szDomain[MAX_PATH];
  DWORD cbName = MAX_PATH, cbDomain = MAX_PATH;
  SID_NAME_USE snu;
  BOOL inconnu = FALSE;

  FILETIME FileTime,LocalFileTime;
  SYSTEMTIME SysTime;
  LONGLONG lgTemp;

  LINE_ITEM lv_line[NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL]];

  unsigned long int i=1,r=0;
  unsigned int taille_tmp;
  char *pStrings;
  BOOL critical;

  strcpy(lv_line[0].c,eventname);

  while (ReadEventLog(Heventlog,EVENTLOG_BACKWARDS_READ |EVENTLOG_SEQUENTIAL_READ,0,&bBuffer,cbBuffer,&dwRead,&dwNeeded) && i < cRecords)
  {
    pevlr = (EVENTLOGRECORD *) &bBuffer;
    if (pevlr!=NULL && pevlr->Length>0)
    {
      while (dwRead > 0 && i < cRecords)
      {
        snprintf(lv_line[1].c,MAX_LINE_SIZE,"%05lu",r++);

        //Type
        inconnu = FALSE;
        switch(pevlr->EventType)
        {
          case 0x01 : strcpy(lv_line[6].c,"ERROR"); break;
          case 0x02 : strcpy(lv_line[6].c,"WARNING"); break;
          case 0x04 : strcpy(lv_line[6].c,"INFORMATION"); break;
          case 0x08 : strcpy(lv_line[6].c,"AUDIT_SUCCESS"); break;
          case 0x10 : strcpy(lv_line[6].c,"AUDIT_FAILURE"); break;
          default :
            inconnu = TRUE;
            lv_line[6].c[0]=0;
          break;
        }

        if (!inconnu)
        {
          //traitement de la date
          lgTemp = Int32x32To64(pevlr->TimeGenerated,10000000) + 116444736000000000;
          FileTime.dwLowDateTime = (DWORD) lgTemp;
          FileTime.dwHighDateTime = (DWORD)(lgTemp >> 32);
          if (FileTimeToLocalFileTime(&FileTime, &LocalFileTime))
          {
            if (FileTimeToSystemTime(&LocalFileTime, &SysTime))
              snprintf(lv_line[3].c,MAX_LINE_SIZE,"%02d/%02d/%02d-%02d:%02d:%02d",SysTime.wYear,SysTime.wMonth,SysTime.wDay,SysTime.wHour,SysTime.wMinute,SysTime.wSecond);
            else lv_line[3].c[0]=0;
          }else lv_line[3].c[0]=0;

          //source
          lv_line[4].c[0]=0;
          if (sizeof(EVENTLOGRECORD) < pevlr->Length)
            strncpy(lv_line[4].c,(char *)pevlr+sizeof(EVENTLOGRECORD),MAX_LINE_SIZE);

          //ID
          if (EventIdtoDscr(pevlr->EventID& 0xFFFF, lv_line[4].c, lv_line[2].c, MAX_LINE_SIZE)){strcpy(lv_line[8].c,"X");critical=TRUE;}
          else {lv_line[8].c[0]=0;critical=FALSE;}

          //description
          memset(lv_line[5].c, 0, MAX_LINE_SIZE);
          taille_tmp = (pevlr->Length) * sizeof(char) - sizeof(EVENTLOGRECORD) - strlen(lv_line[4].c);
          if (taille_tmp  > 0)
          {
            pStrings = (char*)HeapAlloc(GetProcessHeap(), 0, taille_tmp+1);
            if (pStrings != NULL)
            {
              memset(pStrings, 0, taille_tmp);
              memcpy(pStrings, (LPBYTE)pevlr+sizeof(EVENTLOGRECORD)+strlen(lv_line[4].c), taille_tmp);
              TraiterDescriptionLog(pStrings,taille_tmp-1,lv_line[5].c,MAX_LINE_SIZE);

              if (strlen(pStrings)>3)strncpy(lv_line[5].c,pStrings,MAX_LINE_SIZE);
              HeapFree(GetProcessHeap(), 0, pStrings);
            }
          }

          //Utilisateur + SID
          //récupération du nom d'utilisateur  associés a l'évenement
          lpSid = (PSID)((LPBYTE) pevlr + pevlr->UserSidOffset);
          szName[0]=0;
          szDomain[0]=0;
          cbName = MAX_PATH;
          cbDomain = MAX_PATH;
          if (LookupAccountSid(0, lpSid, szName, &cbName, szDomain, &cbDomain, &snu))
          {
            if (cbName>0)
            {
              strncpy(lv_line[7].c,szDomain,MAX_LINE_SIZE);
              strncat(lv_line[7].c,"\\",MAX_LINE_SIZE);
              strncat(lv_line[7].c,szName,MAX_LINE_SIZE);
            }
            else lv_line[7].c[0]=0;
          }else lv_line[7].c[0]=0;

          //ajout du SID
          if (IsValidSid(lpSid))
          {
            PUCHAR pcSubAuth = GetSidSubAuthorityCount(lpSid);
            unsigned char ucMax = *pcSubAuth;
            DWORD *SidP;
            strncat(lv_line[7].c," SID:S-1-5",MAX_LINE_SIZE);

            //récupération des éléments un par un
            szName[0] = 0;
            for (i=0;i<ucMax;++i)
            {
             SidP=GetSidSubAuthority(lpSid,i);
             snprintf(szName,MAX_PATH,"-%d",(int)*SidP);
             strncat(lv_line[7].c,szName,MAX_LINE_SIZE);
            }
          }
          strncat(lv_line[7].c,"\0",MAX_LINE_SIZE);

          AddToLV_log(hlv, lv_line, NB_COLONNE_LV[LV_LOGS_VIEW_NB_COL],critical);
        }
        pevlr = (EVENTLOGRECORD *)((LPBYTE) pevlr + pevlr->Length);
        dwRead = dwRead-pevlr->Length;
        i++;
      }
    }
    break;
  }
  //libération de la DLL
}
//------------------------------------------------------------------------------
void OpenDirectEventLog(char *eventname, HANDLE hlv)
{
  //backup puis event log
  HANDLE Heventlog = OpenBackupEventLog(NULL,eventname);
  DWORD cRecords=0;
  if (Heventlog!=NULL)
  {
    if (GetNumberOfEventLogRecords(Heventlog, &cRecords))
    {
      if (cRecords>0)
      {
        LireEvent(Heventlog,eventname,hlv, cRecords);
        CloseEventLog(Heventlog);
      }
    }
  }

  Heventlog = OpenEventLog(NULL,eventname);
  cRecords=0;
  if (Heventlog!=NULL)
  {
    if (GetNumberOfEventLogRecords(Heventlog, &cRecords))
    {
      if (cRecords>0)
      {
        LireEvent(Heventlog,eventname,hlv, cRecords);
        CloseEventLog(Heventlog);
      }
    }
  }
}
//------------------------------------------------------------------------------
DWORD WINAPI Scan_logs(LPVOID lParam)
{
  //on vide les listeview
  HANDLE hlv        = GetDlgItem(Tabl[TABL_LOGS],LV_LOGS_VIEW);
  if (IsDlgButtonChecked(Tabl[TABL_CONF],CHK_CONF_CLEAN)==BST_CHECKED)ListView_DeleteAllItems(hlv);
  char tmp[MAX_PATH];

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_LOGS,TRUE);

  if ((BOOL)lParam)
  {
    BOOL ok = FALSE;
    //énumération de tous les journaux existants
    // Dans : HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\Eventlog
    char eventname[MAX_PATH];
    HKEY CleTmp=0;
    DWORD nbSubKey=0,i=0, TailleNomSubKey;
    if (RegOpenKey((HKEY)HKEY_LOCAL_MACHINE,"SYSTEM\\CurrentControlSet\\Services\\Eventlog",&CleTmp)==ERROR_SUCCESS)
    {
      if (RegQueryInfoKey (CleTmp,0,0,0,&nbSubKey,0,0,0,0,0,0,0)==ERROR_SUCCESS)
      {
        if (nbSubKey > 0)
        {
          for (i=0;i<nbSubKey;i++)
          {
            TailleNomSubKey = MAX_PATH;
            if (RegEnumKeyEx (CleTmp,i,eventname,(LPDWORD)&TailleNomSubKey,0,0,0,0)==ERROR_SUCCESS)
            {
              ok=TRUE;
              OpenDirectEventLog(eventname, hlv);
            }
          }
        }
      }
      RegCloseKey(CleTmp);
    }

    if (!ok)
    {
      OpenDirectEventLog("Application", hlv);       //journal application
      OpenDirectEventLog("Security", hlv);          //journal sécurité
      OpenDirectEventLog("System", hlv);            //journal système
      OpenDirectEventLog("Internet Explorer", hlv); //Internet Explorer
      OpenDirectEventLog("OSession", hlv);          //session Office
    }

    snprintf(tmp,MAX_PATH,"load %u events",ListView_GetItemCount(hlv));
    SB_add_T(SB_ONGLET_LOGS, tmp);
  }else
  {
    //récupération du 1er fils dans le treeview
    HTREEITEM hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_CHILD, (LPARAM)TRV_HTREEITEM[TRV_LOGS]);

    //on énumère tous les items fils du treeview
    do
    {
      //récupération du texte de l'item
      TVITEM tvitem;
      tmp[0]=0;
      tvitem.hItem = hitem;
      tvitem.mask = TVIF_TEXT;
      tvitem.pszText = tmp;
      tvitem.cchTextMax = MAX_PATH;
      if (SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETITEM,(WPARAM)0, (LPARAM)&tvitem))
      {
        switch(tmp[strlen(tmp)-1])
        {
          case 'G'://fichiers txt au format LOG (type linux)
          case 'g':TraiterlogFile(tmp, hlv);break;
          case 'X':
          case 'x':TraiterEventlogFileEvtx(tmp, hlv);break;
          case 'T':
          case 't':TraiterEventlogFileEvt(tmp, hlv);break;
        }
      }
    }while((hitem = (HTREEITEM)SendDlgItemMessage(Tabl[TABL_CONF],TRV_CONF_TESTS, TVM_GETNEXTITEM,(WPARAM)TVGN_NEXT, (LPARAM)hitem)) && ScanStart);

    snprintf(tmp,MAX_PATH,"load %u events",ListView_GetItemCount(hlv));
    SB_add_T(SB_ONGLET_LOGS, tmp);
  }

  h_scan_logs = NULL;
  if (!h_scan_logs && !h_scan_files && !h_scan_registry && !h_scan_configuration)
  {
    ScanStart = FALSE;
    SetWindowText(GetDlgItem(Tabl[TABL_CONF],BT_CONF_START),"Start");
    SendDlgItemMessage(Tabl[TABL_MAIN],SB_MAIN,SB_SETTEXT,(WPARAM)SB_ONGLET_INFO, (LPARAM)"");
  }

  MiseEnGras(Tabl[TABL_MAIN],BT_MAIN_LOGS,FALSE);
  return 0;
}
