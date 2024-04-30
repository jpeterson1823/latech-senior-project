-- MySQL Script generated by MySQL Workbench
-- Sat Apr 27 22:18:59 2024
-- Model: New Model    Version: 1.0
-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,NO_ZERO_IN_DATE,NO_ZERO_DATE,ERROR_FOR_DIVISION_BY_ZERO,NO_ENGINE_SUBSTITUTION';

-- -----------------------------------------------------
-- Schema PRISM_DB
-- -----------------------------------------------------

-- -----------------------------------------------------
-- Schema PRISM_DB
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `PRISM_DB` DEFAULT CHARACTER SET utf8 ;
USE `PRISM_DB` ;

-- -----------------------------------------------------
-- Table `PRISM_DB`.`Users`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PRISM_DB`.`Users` (
  `idUsers` INT NOT NULL AUTO_INCREMENT,
  `Username` VARCHAR(255) NOT NULL,
  `Password` VARCHAR(255) NOT NULL,
  `model_path` VARCHAR(255) NULL,
  `audio_path` VARCHAR(255) NULL,
  `calendar_path` VARCHAR(255) NULL,
  PRIMARY KEY (`idUsers`),
  UNIQUE INDEX `Username_UNIQUE` (`Username` ASC) VISIBLE,
  UNIQUE INDEX `idUsers_UNIQUE` (`idUsers` ASC) VISIBLE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PRISM_DB`.`GUI_Data`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PRISM_DB`.`GUI_Data` (
  `idGUI_Data` INT NOT NULL AUTO_INCREMENT,
  `task` TEXT NULL,
  `completed` TEXT NULL,
  `date` TEXT NULL,
  `Users_idUsers` INT NOT NULL,
  PRIMARY KEY (`idGUI_Data`, `Users_idUsers`),
  UNIQUE INDEX `idGUI_Data_UNIQUE` (`idGUI_Data` ASC) VISIBLE,
  INDEX `fk_GUI_Data_Users1_idx` (`Users_idUsers` ASC) VISIBLE,
  CONSTRAINT `fk_GUI_Data_Users1`
    FOREIGN KEY (`Users_idUsers`)
    REFERENCES `PRISM_DB`.`Users` (`idUsers`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PRISM_DB`.`Hardware`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PRISM_DB`.`Hardware` (
  `idHardware` INT NOT NULL AUTO_INCREMENT,
  `sensor_name` VARCHAR(255) NOT NULL,
  `description` VARCHAR(255) NULL,
  `macaddr` VARCHAR(18) NOT NULL,
  `ipaddr` VARCHAR(16) NOT NULL,
  `setting1` VARCHAR(255) NOT NULL DEFAULT 'null',
  `setting2` VARCHAR(255) NOT NULL DEFAULT 'null',
  `setting3` VARCHAR(255) NOT NULL DEFAULT 'null',
  `changed` TINYINT(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`idHardware`),
  UNIQUE INDEX `idHardware_UNIQUE` (`idHardware` ASC) VISIBLE,
  UNIQUE INDEX `macaddr_UNIQUE` (`macaddr` ASC) VISIBLE)
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `PRISM_DB`.`Users_has_Hardware`
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS `PRISM_DB`.`Users_has_Hardware` (
  `Users_idUsers` INT NOT NULL,
  `Hardware_idHardware` INT NOT NULL,
  PRIMARY KEY (`Users_idUsers`, `Hardware_idHardware`),
  INDEX `fk_Users_has_Hardware_Hardware1_idx` (`Hardware_idHardware` ASC) VISIBLE,
  INDEX `fk_Users_has_Hardware_Users_idx` (`Users_idUsers` ASC) VISIBLE,
  CONSTRAINT `fk_Users_has_Hardware_Users`
    FOREIGN KEY (`Users_idUsers`)
    REFERENCES `PRISM_DB`.`Users` (`idUsers`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION,
  CONSTRAINT `fk_Users_has_Hardware_Hardware1`
    FOREIGN KEY (`Hardware_idHardware`)
    REFERENCES `PRISM_DB`.`Hardware` (`idHardware`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;
