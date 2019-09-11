-- phpMyAdmin SQL Dump
-- version 4.2.7.1
-- http://www.phpmyadmin.net
--
-- Host: 127.0.0.1
-- Generation Time: Lug 14, 2019 alle 17:48
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
CREATE DATABASE IF NOT EXISTS `web_editor` DEFAULT CHARACTER SET latin1 COLLATE latin1_swedish_ci;
USE `web_editor`;

-- --------------------------------------------------------

--
-- Struttura della tabella `documenti`
--

CREATE TABLE IF NOT EXISTS `documenti` (
`DocId` int(11) NOT NULL,
  `FileSysPath` varchar(128) NOT NULL,
  `LinkPath` varchar(128) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1 AUTO_INCREMENT=1 ;

-- --------------------------------------------------------

--
-- Struttura della tabella `relazioni`
--

CREATE TABLE IF NOT EXISTS `relazioni` (
  `RId` int(11) NOT NULL,
  `UserId` int(11) NOT NULL,
  `DocId` int(11) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

--
-- Struttura della tabella `utenti`
--

CREATE TABLE IF NOT EXISTS `utenti` (
`UserId` int(11) NOT NULL,
  `UserName` varchar(64) NOT NULL,
  `NickName` varchar(32) NOT NULL,
  `PasswordCript` varchar(32) NOT NULL
) ENGINE=InnoDB  DEFAULT CHARSET=latin1 AUTO_INCREMENT=2 ;

--
-- Dump dei dati per la tabella `utenti`
--

INSERT INTO `utenti` (`UserId`, `UserName`, `NickName`, `PasswordCript`) VALUES
(1, 'user1@asd.it', 'user1', '0');

--
-- Indexes for dumped tables
--

--
-- Indexes for table `documenti`
--
ALTER TABLE `documenti`
 ADD PRIMARY KEY (`DocId`);

--
-- Indexes for table `relazioni`
--
ALTER TABLE `relazioni`
 ADD PRIMARY KEY (`RId`,`UserId`,`DocId`);

--
-- Indexes for table `utenti`
--
ALTER TABLE `utenti`
 ADD PRIMARY KEY (`UserId`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `documenti`
--
ALTER TABLE `documenti`
MODIFY `DocId` int(11) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `utenti`
--
ALTER TABLE `utenti`
MODIFY `UserId` int(11) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
