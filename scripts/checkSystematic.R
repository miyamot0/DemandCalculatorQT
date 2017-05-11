
myArgs <- commandArgs(trailingOnly = TRUE)

mModel <- myArgs[1]
mIds <- as.numeric(unlist(strsplit(myArgs[2], split=",")))
mPrices <- as.numeric(unlist(strsplit(myArgs[3], split=",")))
mConsume <- as.numeric(unlist(strsplit(myArgs[4], split=",")))

dat <- data.frame(id=as.numeric(mIds), x=as.numeric(mPrices), y=as.numeric(mConsume))

options(warn=-1)

if(!suppressMessages(require(jsonlite))){
  suppressMessages(install.packages("jsonlite"))
  if(!require(jsonlite)){
    suppressMessages(install.packages("jsonlite"))
    suppressMessages(library(jsonlite))
  }
}

if(!suppressMessages(require(beezdemand))){
  suppressMessages(install.packages("beezdemand"))
  if(!require(beezdemand)){
    suppressMessages(install.packages("beezdemand"))
    suppressMessages(library(beezdemand))
  }
}

printSystematic <- CheckUnsystematic(dat, deltaq = 0.025, bounce = 0.1, reversals = 0, ncons0 = 2)
printSystematic <- toJSON(printSystematic, pretty = FALSE)
printSystematic
