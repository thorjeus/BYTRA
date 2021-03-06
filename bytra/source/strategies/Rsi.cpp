//
// Created by Arne Wouters on 02/08/2020.
//

#include "Rsi.h"

#include <spdlog/spdlog.h>
#include <ta-lib/ta_libc.h>

Rsi::Rsi() {
    /** RSI strategy
     * The relative strength index (RSI) is most commonly used to indicate temporarily
     * overbought or oversold conditions in a market.
     * The RSI is a widely used technical indicator and an oscillator
     * that indicates a market is overbought when the RSI value is over 70 and indicates
     * oversold conditions when RSI readings are under 30.
     * We buy when the market is oversold and sell when the market is overbought.
     * */
    name = "RSI";
    timeframes = {{"1", 1000}};
    symbol = "BTCUSD";
    qty = 100;
    orderType = "Limit";
    slippage = 5.0;
    stopLossPercentage = 0.03;
}

bool Rsi::checkLongEntry(std::map<TimeFrame, std::vector<std::shared_ptr<Candle>>> &candles) {
    auto tf = TimeFrame(timeframes[0].first, timeframes[0].second);
    double rsi_value = calculateRSI(candles[tf]);

    if (rsi_value < 30) {
        spdlog::debug("Entry Long rsi={}", rsi_value);
        return true;
    }

    return false;
}

bool Rsi::checkShortEntry(std::map<TimeFrame, std::vector<std::shared_ptr<Candle>>> &candles) {
    auto tf = TimeFrame(timeframes[0].first, timeframes[0].second);
    double rsi_value = calculateRSI(candles[tf]);

    if (rsi_value > 70) {
        spdlog::debug("Entry Short rsi={}", rsi_value);
        return true;
    }

    return false;
}

bool Rsi::checkExit(std::map<TimeFrame, std::vector<std::shared_ptr<Candle>>> &candles,
                    std::shared_ptr<Position> position) {
    auto tf = TimeFrame(timeframes[0].first, timeframes[0].second);
    double rsi_value = calculateRSI(candles[tf]);

    if ((rsi_value > 50 && position->isLong()) || (rsi_value < 50 && position->isShort())) {
        spdlog::debug("Exit rsi={}", rsi_value);
        return true;
    }

    return false;
}

double Rsi::calculateRSI(std::vector<std::shared_ptr<Candle>> &candles) {
    std::vector<double> close;
    close.reserve(candles.size());

    for (auto &candle : candles) {
        close.push_back(candle->close);
    }

    int endIdx = (int)close.size() - 1;
    int startIdx = endIdx - timeframes[0].second + 1;
    int rsi_length = 10;
    int outBegIdx;
    int outNbElement;
    std::vector<double> rsi(close.size() - rsi_length);

    TA_RetCode retCode = TA_RSI(startIdx, endIdx, close.data(), rsi_length, &outBegIdx, &outNbElement, rsi.data());

    double rsi_value = rsi[outNbElement - 1];

    return rsi_value;
}
