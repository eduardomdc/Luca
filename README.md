# Luca - Local UDP Chat Alternative
Luca provides the simplest way to send messages between users on the same local network. Its design philosophy is centered around open, non-anonymized, non-cryptographic communication between trusted network peers.

Luca is a serverless chat application that operates using UDP packets in broadcast mode. This means that when you send a message through Luca, everyone on the network will receive it. You don’t need to know a server’s IP address to communicate with other users, as all devices on the local network are continuously listening.

Luca utilizes a simple application layer protocol built upon UDP which allows users to know which other users are online and when they sign off. Users are also represented with nicknames and chosen colors stored in each running Luca client. This means there's no localized Luca node. Each client acts as their own server.