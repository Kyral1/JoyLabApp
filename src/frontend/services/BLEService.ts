import { BleManager, Device, Characteristic } from 'react-native-ble-plx';
import { Buffer } from 'buffer';

const TOY_NAME = 'JoyLabToy';
const SERVICE_UUID = '00001234-0000-1000-8000-00805f9b34fb';
const CTRL_UUID = '0000abcd-0000-1000-8000-00805f9b34fb';
const SETT_UUID = '0000abce-0000-1000-8000-00805f9b34fb';
const EVTS_UUID = '0000abcf-0000-1000-8000-00805f9b34fb';

class BLEService {
  manager: BleManager;
  device: Device | null = null;
  controlChar: Characteristic | null = null;

  constructor() {
    this.manager = new BleManager();
  }

  async connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      const timeout = setTimeout(() => {
        this.manager.stopDeviceScan();
        reject(new Error('Device not found'));
      }, 8000);

      this.manager.startDeviceScan(null, null, async (error, d) => {
        if (error) {
          clearTimeout(timeout);
          this.manager.stopDeviceScan();
          return reject(error);
        }
        if (d?.name === TOY_NAME) {
          clearTimeout(timeout);
          this.manager.stopDeviceScan();
          try {
            await d.connect();
            await d.discoverAllServicesAndCharacteristics();
            this.device = d;

            const chars = await d.characteristicsForService(SERVICE_UUID);
            this.controlChar = chars.find(c => c.uuid.toLowerCase() === CTRL_UUID) || null;
            resolve();
          } catch (e) { reject(e); }
        }
      });
    });
  }

  async sendControlFrame(bytes: number[]) {
    if (!this.controlChar) throw new Error('Not connected');
    const frame = Buffer.from(bytes).toString('base64');
    await this.controlChar.writeWithoutResponse(frame);
  }
}

export const bleService = new BLEService();
