myArgs <- commandArgs(trailingOnly = TRUE)

mModel <- myArgs[1]
mIds <- as.numeric(unlist(strsplit(myArgs[2], split=",")))
mPrices <- as.numeric(unlist(strsplit(myArgs[3], split=",")))
mConsume <- as.numeric(unlist(strsplit(myArgs[4], split=",")))
mParamK <- myArgs[5]
mRem0 <- myArgs[6]
mReplNum <- myArgs[7]
mRemQ0 <- myArgs[8]
mReplQ0 <- myArgs[9]
mPlotFlag <- myArgs[10]

dat <- data.frame(id=as.numeric(mIds), x=as.numeric(mPrices), y=as.numeric(mConsume))

if (mRem0 == "DROP") {
    mRem0 <- TRUE
    mReplNum <- NULL
} else if (mRem0 == "KEEP") {
    mRem0 <- FALSE
    mReplNum <- 0
} else if (mRem0 == "MODIFY") {
    mRem0 <- FALSE
    mReplNum <- as.numeric(mReplNum)
}

if (mRemQ0 == "DROP") {
    mRemQ0 <- TRUE
    mReplQ0 <- NULL
} else if (mRemQ0 == "KEEP") {
    mRemQ0 <- FALSE
    mReplQ0 <- 0.01
} else if (mRemQ0 == "MODIFY") {
    mRemQ0 <- TRUE
    mReplQ0 <- as.numeric(mReplQ0)
}

options(warn=-1)

suppressMessages(require(jsonlite))
suppressMessages(require(beezdemand))
suppressMessages(require(ggplot2))

PlotCurve <- function(adf, dfrow, fitfail) {
  yMinCoord <- 0.01
  yMaxCoord <- max(adf$y)*2

  yBreaks <- c(0.1, 1, 10, 100, 1000, 10000, 100000)
  xBreaks <- c(0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000)

  xModBreaks <- c(0.00001,  0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000)
  xModLabels <- c("QFree",  0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 100000)

  if (!fitfail) {
    segmentFrame <- data.frame(x1 = c(0),
                               x2 = c(0),
                               y1 = c(0),
                               y2 = c(0))

    segmentFrame[1, "x1"] <- dfrow[["Pmaxd"]]
    segmentFrame[1, "x2"] <- dfrow[["Pmaxd"]]
    segmentFrame[1, "y1"] <- 0

    lowPrice <- 0.001

    # Lengthen out the curve domain
    highPrice <- max(adf$x) * 2

    if (highPrice < dfrow[["Pmaxd"]]) {
        highPrice <- dfrow[["Pmaxd"]]
    }

    xSeries <- seq(from = lowPrice, to = highPrice, by = 0.001)
    ySeries <- rep(NA, length(xSeries))

    if (dfrow[["Equation"]] == "hs") {
      ySeries <- (log(dfrow[["Q0d"]])/log(10)) + dfrow[["K"]] * (exp(-dfrow[["Alpha"]] * dfrow[["Q0d"]] * xSeries) - 1)
      ySeries <- 10^ySeries

      segmentFrame[1, "y2"] <- 10^((log(dfrow[["Q0d"]])/log(10)) + dfrow[["K"]] * (exp(-dfrow[["Alpha"]] * dfrow[["Q0d"]] * dfrow[["Pmaxd"]]) - 1))

    } else if (dfrow[["Equation"]] == "koff") {
      ySeries <- dfrow[["Q0d"]] * 10^(dfrow[["K"]] * (exp(-dfrow[["Alpha"]] * dfrow[["Q0d"]] * xSeries) - 1))

      segmentFrame[1, "y2"] <- dfrow[["Q0d"]] * 10^(dfrow[["K"]] * (exp(-dfrow[["Alpha"]] * dfrow[["Q0d"]] * dfrow[["Pmaxd"]]) - 1))
    }

    tempnew <- data.frame(x=xSeries,
                          y=ySeries)

    pointFrame <- data.frame(X=adf$x, Y=adf$y)

    if (0 %in% pointFrame$X) {
      # Split axes are warranted here

      pointFrame$mask <- 1
      tempnew$mask <- 1

      pointFrame[pointFrame$X == 0,]$mask <- 0
      pointFrame[pointFrame$X == 0,]$X <- 0.00001

      segmentFrame$mask <- 1

      logChart <- ggplot(pointFrame,aes(x=X,y=Y)) +
        geom_point(size=2, shape=21, show.legend=T) +
        geom_segment(aes(x = x1, y = y1, xend = x2, yend = y2), show.legend = F, data = segmentFrame, linetype=2) +
        facet_grid(.~mask, scales="free_x", space="free_x") +
        geom_line(data=tempnew, aes(x=x, y=y)) +
        scale_x_log10(breaks=xModBreaks,
                      labels=xModLabels) +
        scale_y_log10(breaks=yBreaks,
                      labels=yBreaks) +
        coord_cartesian(ylim=c(yMinCoord, yMaxCoord)) +
        ggtitle(paste("Participant", dfrow[["ID"]], sep = "-")) +
        theme(strip.background = element_blank(),
              strip.text = element_blank(),
              panel.background = element_blank(),
              panel.grid.major = element_blank(),
              panel.grid.minor = element_blank(),
              panel.border = element_rect(colour = "white",
                                          fill=FALSE,
                                          size=0),
              axis.line.x = element_line(colour = "black"),
              axis.line.y = element_line(colour = "black"),
              axis.text.x=element_text(colour="black"),
              axis.text.y=element_text(colour="black"),
              text = element_text(size=16),
              plot.title = element_text(hjust = 0.5),
              legend.position = "bottom",
              legend.title=element_blank(),
              legend.key = element_rect(fill = "transparent", colour = "transparent")) +
        labs(x = "Price", y = "Reported Consumption")

    } else {
      # Regular representation

      logChart <- ggplot(pointFrame,aes(x=X,y=Y)) +
        geom_point(size=2, shape=21, show.legend=T) +
        geom_segment(aes(x = x1, y = y1, xend = x2, yend = y2), show.legend = F, data = segmentFrame, linetype=2) +
        geom_line(data=tempnew, aes(x=x, y=y)) +
        scale_x_log10(breaks=xBreaks,
                      labels=xBreaks) +
        scale_y_log10(breaks=yBreaks,
                      labels=yBreaks) +
        coord_cartesian(ylim=c(yMinCoord, yMaxCoord)) +
        ggtitle(paste("Participant", dfrow[["ID"]], sep = "-")) +
        theme(panel.background = element_blank(),
              panel.grid.major = element_blank(),
              panel.grid.minor = element_blank(),
              panel.border = element_rect(colour = "white",
                                          fill=FALSE,
                                          size=0),
              axis.line.x = element_line(colour = "black"),
              axis.line.y = element_line(colour = "black"),
              axis.text.x=element_text(colour="black"),
              axis.text.y=element_text(colour="black"),
              text = element_text(size=16),
              plot.title = element_text(hjust = 0.5),
              legend.title=element_blank(),
              legend.position = "bottom",
              legend.key = element_rect(fill = "transparent",
                                        colour = "transparent")) +
        labs(x = "Price", y = "Reported Consumption")
    }

    logChart

  } else {
    # fitting failed in these instances

    pointFrame <- data.frame(X=adf$x, Y=adf$y)

    if (0 %in% pointFrame$X) {
      # Split axes are warranted
      pointFrame$mask <- 1

      pointFrame[pointFrame$X == 0,]$mask <- 0
      pointFrame[pointFrame$X == 0,]$X <- 0.00001

      logChart <- ggplot(pointFrame,aes(x=X,y=Y)) +
        geom_point(size=2, shape=21, show.legend=T) +
        geom_blank(data = data.frame(X=0.001,
                                     Y=0.001,
                                     mask=1)) +
        geom_blank(data = data.frame(X=max(adf$x)*2,
                                     Y=max(adf$y),
                                     mask=1)) +
        facet_grid(.~mask, scales="free_x", space="free_x") +
        scale_x_log10(breaks=xModBreaks,
                      labels=xModLabels) +
        scale_y_log10(breaks=yBreaks,
                      labels=yBreaks) +
        coord_cartesian(ylim=c(yMinCoord, yMaxCoord)) +
        ggtitle(paste("Participant", dfrow[["ID"]], sep = "-")) +
        theme(strip.background = element_blank(),
              strip.text = element_blank(),
              panel.background = element_blank(),
              panel.grid.major = element_blank(),
              panel.grid.minor = element_blank(),
              panel.border = element_rect(colour = "white",
                                          fill=FALSE,
                                          size=0),
              axis.line.x = element_line(colour = "black"),
              axis.line.y = element_line(colour = "black"),
              axis.text.x=element_text(colour="black"),
              axis.text.y=element_text(colour="black"),
              text = element_text(size=16),
              plot.title = element_text(hjust = 0.5),
              legend.position = "bottom",
              legend.title=element_blank(),
              legend.key = element_rect(fill = "transparent", colour = "transparent")) +
        labs(x = "Price", y = "Reported Consumption")

    } else {
      # Regular representation

      logChart <- ggplot(pointFrame,aes(x=X,y=Y)) +
        geom_point(size=2, shape=21, show.legend=T) +
        geom_blank(data = data.frame(X=0.001,
                                     Y=0.001)) +
        geom_blank(data = data.frame(X=max(adf$x)*2,
                                     Y=max(adf$y))) +
        scale_x_log10(breaks=xBreaks,
                      labels=xBreaks) +
        scale_y_log10(breaks=yBreaks,
                      labels=yBreaks) +
        coord_cartesian(ylim=c(yMinCoord, yMaxCoord)) +
        ggtitle(paste("Participant", dfrow[["ID"]], sep = "-")) +
        theme(panel.background = element_blank(),
              panel.grid.major = element_blank(),
              panel.grid.minor = element_blank(),
              panel.border = element_rect(colour = "white",
                                          fill=FALSE,
                                          size=0),
              axis.line.x = element_line(colour = "black"),
              axis.line.y = element_line(colour = "black"),
              axis.text.x=element_text(colour="black"),
              axis.text.y=element_text(colour="black"),
              text = element_text(size=16),
              plot.title = element_text(hjust = 0.5),
              legend.title=element_blank(),
              legend.position = "bottom",
              legend.key = element_rect(fill = "transparent", colour = "transparent")) +
        labs(x = "Price", y = "Reported Consumption")

    }

    logChart
  }
}

### Clean up data, per user req's
dat2 <- beezdemand::ChangeData(dat, rem0 = mRem0, replnum = mReplNum, remq0e = mRemQ0, replfree = mReplQ0)

### Regex, to determine nature of command
numberCheck <- function(x) !grepl("\\D", x)

### Value check, for parameter peek
valueCheck <- function(x) is.numeric(x) & !is.na(x)

### Reference to results
fitDemand <- NULL

if (mParamK != "NULL") {
    if(numberCheck(mParamK)) {
        ### K is a numeric assignment
        fitDemand <- suppressMessages(FitCurves(dat2, mModel, k = as.numeric(mParamK)))
    } else {
        ### K is a string command
        fitDemand <- suppressMessages(FitCurves(dat2, mModel, k = mParamK))
    }
}

fitDemand$figure = "SKIP"

if (mPlotFlag == "TRUE") {
    for (i in 1:nrow(fitDemand)) {
        mIndDat <- dat[dat$id==i,]
        row <- fitDemand[i,]

        skipPlotting <- FALSE

        if (row[["Equation"]] == "linear") {
            skipPlotting <- TRUE
        } else if (!valueCheck(row[["K"]])) {
            skipPlotting <- TRUE
        }

        mPlot <- PlotCurve(mIndDat, row, skipPlotting)

        svg(tempf2 <- tempfile(fileext = '.svg'), width=7, height=7)
        print(mPlot)
        dev.off()

        fitDemand[i,]$figure <- base64enc::base64encode(readBin(tempf2, 'raw', file.info(tempf2)[1, 'size']), 'txt')

        unlink(tempf2)
    }
}

fitDemand <- toJSON(fitDemand, pretty = FALSE)
fitDemand
