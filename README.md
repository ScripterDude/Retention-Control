# Retention Control

**Retention Control** is a Windows application for automatically cleaning up files based on folder-specific retention rules.

You can select a folder, define how long files should be kept, and run a synchronization. Retention Control keeps track of files it discovers and removes them once they have passed the configured expiration period.

This project was primarily built as a way for me to practice **C++ fundamentals** while also getting a sense of how to build a native Windows application with **WinUI 3** and the **Windows App SDK**.

> [!WARNING]
> This is a learning project and is still in development.
> The current implementation **permanently deletes expired files** rather than moving them to the Windows Recycle Bin. Do not use it on important folders without understanding what a rule will do.

## Features

* Create retention rules for individual folders
* Configure an expiration period in days
* Track files discovered inside configured folders
* Store rule and file-tracking data between application launches
* Manually synchronize all configured rules
* Run synchronization work away from the UI thread
* Remove files that have exceeded their retention period
* Keep a log of removed files
* Edit and remove existing rules
* Display active rule and removed-file counts
* Windows notifications with synchronization progress
* System tray support
* Mica and Acrylic backdrop options

## How it works

A rule consists of a folder, an expiration period, and file-type information.

When a synchronization is started, Retention Control processes each configured rule and scans its folder.

New files are registered in the application's local cache along with the date on which they were first discovered. On later synchronizations, Retention Control compares that discovery date against the rule's expiration period.

For example, with a rule such as:

```text
Folder: C:\Users\Example\Downloads\Temporary
Expiration: 30 days
```

a newly discovered file is first registered by Retention Control. Once it has been tracked for at least 30 days, a later synchronization can identify it as expired and remove it.

This means the retention period is currently based on **when Retention Control first discovers a file**, rather than the file's creation or modification timestamp.

## Why I made this

The main goal of this project was to improve my understanding of **C++ by building something practical** instead of only working through small exercises.

The parts I particularly wanted to practice include:

* Working with `std::filesystem`
* Structs and data models
* Reading and writing configuration files
* Persisting application state
* Generating, loading, editing, and deleting rules
* Tracking files between scans
* Date and expiration calculations
* Separating application logic from UI logic
* Running synchronization work in the background without blocking the UI
* Working with native Windows APIs from C++
* Error handling and application state

WinUI 3 provided a useful way to put that C++ logic behind an actual desktop application.

## UI and learning resources

The application started from the **WinUI 3 Blank App** template rather than from a pre-built application.

For the interface, I used controls, layouts, and design ideas from Microsoft's official **WinUI 3 Gallery** and adapted them for Retention Control.

I also relied heavily on **Microsoft Learn** and the Windows App SDK / WinUI documentation while learning how things such as navigation, dialogs, window behavior, notifications, backdrops, pickers, and C++/WinRT work.

## AI assistance disclosure

**GitHub Copilot was used as a development assistant during this project.**

Its assistance was focused on the **WinUI 3 / Windows-specific side of the application**, including UI code, XAML, Windows APIs, and some of the boilerplate involved in connecting a native C++ application to the Windows App SDK.

I used Copilot to help me understand unfamiliar APIs, generate or adapt UI code, and troubleshoot some Windows-specific implementation details.

The main reason I built this project, however, was to practice writing and understanding the underlying **C++ application logic myself**. This includes areas such as the synchronization flow, folder/file processing, retention logic, rule handling, configuration/cache storage, and general program structure.

Where possible, I tried to understand and modify generated suggestions instead of treating them as code to copy blindly.

## Built with

* **C++**
* **C++/WinRT**
* **WinUI 3**
* **Windows App SDK**
* **XAML**
* **Win32 APIs**
* `std::filesystem`

## Current project status

Retention Control is currently a **learning / experimental project** rather than production-ready file-management software.

There are still areas I would like to improve, including:

* Giving rules the option of moving expired files to the Recycle Bin instead of permanently deleting them
* More robust error handling
* Better file-type filtering
* Additional rule options
* Automatic/scheduled synchronization
* Improved handling of inaccessible or deleted folders
* More validation around stored configuration data
* General cleanup and refactoring.

## Disclaimer

Retention Control can delete files.

This software is provided as a learning project with no guarantee that it will behave correctly in every situation. If you experiment with it, use test folders and files that you are prepared to lose.

---

Built as a C++ learning project while exploring native Windows development with WinUI 3.
