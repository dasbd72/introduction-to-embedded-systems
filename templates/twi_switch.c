#include <avr/interrupt.h>
#include <util/twi.h>

ISR(TWI_vect) {
    switch (TW_STATUS) {
        // NOTE: SLA+W = SLave Address + Write bit
        //       SLA+R = SLave Address + Read bit

        // All
        case TW_NO_INFO:    // No state information available
        case TW_BUS_ERROR:  // Bus error; Illegal START or STOP condition

        // All Master
        case TW_START:      // Start condition transmitted
        case TW_REP_START:  // Repeated start condition transmitted

        // Master Transmitter
        case TW_MT_SLA_ACK:    // SLA+W transmitted, ACK received = Slave receiver ACKed address
        case TW_MT_SLA_NACK:   // SLA+W transmitted, NACK received = Slave receiver with transmitted address doesn't exists?
        case TW_MT_DATA_ACK:   // Data transmitted, ACK received = Slave receiver ACKed data
        case TW_MT_DATA_NACK:  // Data transmitted, NACK received
        case TW_MT_ARB_LOST:   // Arbitration lost in SLA+W or data

        // Master Receiver
        case TW_MR_SLA_ACK:    // SLA+R transmitted, ACK received = Slave transmitter ACKed address
        case TW_MR_SLA_NACK:   // SLA+R transmitted, NACK received = Slave transmitter with transmitted address doesn't exists?
        case TW_MR_DATA_ACK:   // Data received, ACK returned
        case TW_MR_DATA_NACK:  // Data received, NACK returned
        // case TW_MR_ARB_LOST:   // Arbitration lost in SLA+R or NACK

        // Slave Receiver
        case TW_SR_SLA_ACK:             // SLA+W received, ACK returned = Addressed, returned ACK
        case TW_SR_GCALL_ACK:           // General call received, ACK returned = Addressed generally, returned ACK
        case TW_SR_ARB_LOST_SLA_ACK:    // Arbitration lost in SLA+RW, SLA+W received, ACK returned
        case TW_SR_ARB_LOST_GCALL_ACK:  // Arbitration lost in SLA+RW, general call received, ACK returned
        case TW_SR_DATA_ACK:            // Data received, ACK returned
        case TW_SR_DATA_NACK:           // Data received, NACK returned
        case TW_SR_GCALL_DATA_ACK:      // General call data received, ACK returned
        case TW_SR_GCALL_DATA_NACK:     // General call data received, NACK returned
        case TW_SR_STOP:                // Stop or repeated start condition received while selected

        // Slave Transmitter
        case TW_ST_SLA_ACK:           // SLA+R received, ACK returned = Addressed, returned ACK
        case TW_ST_ARB_LOST_SLA_ACK:  // Arbitration lost in SLA+RW, SLA+R received, ACK returned
        case TW_ST_DATA_ACK:          // Data transmitted, ACK received
        case TW_ST_DATA_NACK:         // Data transmitted, NACK received = Received NACK, so we are done
        case TW_ST_LAST_DATA:         // Last data byte transmitted, ACK received = Received ACK, but we are done already!
    }
}