-- phpMyAdmin SQL Dump
-- version 4.2.7.1
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Apr 07, 2020 alle 14:22
-- Versione del server: 5.6.20
-- PHP Version: 5.5.15

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `web_editor`
--

-- --------------------------------------------------------

--
-- Struttura della tabella `documenti`
--

CREATE TABLE IF NOT EXISTS `documenti` (
`DocId` int(11) NOT NULL,
  `FileSysPath` varchar(128) NOT NULL,
  `LinkPath` varchar(128) NOT NULL
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=24 ;

--
-- Dump dei dati per la tabella `documenti`
--

INSERT INTO `documenti` (`DocId`, `FileSysPath`, `LinkPath`) VALUES
(1, 'file.txt', 'mioFile'),
(2, 'file1.txt', 'asd_file1'),
(3, 'file2.txt', 'asd_file2'),
(4, 'file_remoto.txt', 'file_remoto.txt'),
(22, 'asd', 'asd'),
(23, 'nuovoF', 'nuovoF');

-- --------------------------------------------------------

--
-- Struttura della tabella `relazioni`
--

CREATE TABLE IF NOT EXISTS `relazioni` (
`RId` int(11) NOT NULL,
  `DocId` int(11) NOT NULL,
  `UserName` varchar(64) NOT NULL
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=7 ;

--
-- Dump dei dati per la tabella `relazioni`
--

INSERT INTO `relazioni` (`RId`, `DocId`, `UserName`) VALUES
(1, 1, 'user'),
(2, 22, 'user'),
(3, 1, 'user1'),
(4, 22, 'user1'),
(5, 1, 'asd'),
(6, 23, 'user');

-- --------------------------------------------------------

--
-- Struttura della tabella `utenti`
--

CREATE TABLE IF NOT EXISTS `utenti` (
  `UserName` varchar(64) NOT NULL,
  `NickName` varchar(32) NOT NULL,
  `Password` varchar(64) NOT NULL,
  `connesso` tinyint(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dump dei dati per la tabella `utenti`
--

INSERT INTO `utenti` (`UserName`, `NickName`, `Password`, `connesso`) VALUES
('asd', 'asd', 'dd121e36961a04627eacff629765dd3528471ed745c1e32222db4a8a5f3421c4', 0),
('pippo', 'pippo', 'dd121e36961a04627eacff629765dd3528471ed745c1e32222db4a8a5f3421c4', 0),
('qwe', 'qwe', 'dd121e36961a04627eacff629765dd3528471ed745c1e32222db4a8a5f3421c4', 0),
('user', 'user', 'dd121e36961a04627eacff629765dd3528471ed745c1e32222db4a8a5f3421c4', 0),
('user1', 'user1', 'dd121e36961a04627eacff629765dd3528471ed745c1e32222db4a8a5f3421c4', 0);

--
-- Indexes for dumped tables
--

--
-- Indexes for table `documenti`
--
ALTER TABLE `documenti`
 ADD UNIQUE KEY `DocId` (`DocId`);

--
-- Indexes for table `relazioni`
--
ALTER TABLE `relazioni`
 ADD UNIQUE KEY `RId` (`RId`), ADD KEY `docid` (`DocId`), ADD KEY `username` (`UserName`);

--
-- Indexes for table `utenti`
--
ALTER TABLE `utenti`
 ADD PRIMARY KEY (`UserName`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `documenti`
--
ALTER TABLE `documenti`
MODIFY `DocId` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=24;
--
-- AUTO_INCREMENT for table `relazioni`
--
ALTER TABLE `relazioni`
MODIFY `RId` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=7;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
