import { BleManager, Device, Characteristic } from 'react-native-ble-plx';
import { Buffer } from 'buffer';
import { PermissionsAndroid, Platform } from 'react-native';


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

  async requestBlePermissions() {
    if (Platform.OS === 'android' && Platform.Version >= 31) {
      const result = await PermissionsAndroid.requestMultiple([
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
        PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      ]);
      console.log('BLE Permissions:', result);
    }
  }

  async connect(): Promise<void> {
    await this.requestBlePermissions();
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

  eventChar: Characteristic | null = null;
  notifySub: { remove?: () => void } | null = null;

  // Subscribe to notifications from the EVTS characteristic
  async enableNotifications(callback: (data: string) => void) {
    if (!this.device) throw new Error('Not connected');

    const chars = await this.device.characteristicsForService(SERVICE_UUID);
    const evtChar = chars.find(c => c.uuid.toLowerCase() === EVTS_UUID);
    if (!evtChar) throw new Error('Events characteristic not found');

    this.eventChar = evtChar;

    // Subscribe for notifications
    this.notifySub = evtChar.monitor((error, characteristic) => {
      if (error) {
        console.error('BLE notify error:', error);
        return;
      }
      if (characteristic?.value) {
        callback(characteristic.value); // base64-encoded string
      }
    });

    console.log('BLE notifications enabled for Events characteristic');
  }

  // Stop notifications (optional cleanup)
  disableNotifications() {
    if (this.notifySub) {
      this.notifySub.remove?.();
      this.notifySub = null;
      console.log('BLE notifications disabled');
    }
  }

  // Helper for React components
  onNotify(callback: (data: string) => void) {
    if (!this.eventChar) {
      console.warn('No event characteristic yet — notifications not active');
      return { remove: () => {} };
    }
    // Return the subscription handle
    return this.eventChar.monitor((error, characteristic) => {
      if (error) {
        console.error('BLE notify error:', error);
        return;
      }
      if (characteristic?.value) callback(characteristic.value);
    });
  }

}

export const bleService = new BLEService();
