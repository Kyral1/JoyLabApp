import { BleManager, Device, Characteristic } from 'react-native-ble-plx';
import { Buffer } from 'buffer';
import { PermissionsAndroid, Platform } from 'react-native';

// Wristband BLE identifiers
const WRISTBAND_NAME = 'JoyLabWristband';
const SERVICE_UUID = '00002234-0000-1000-8000-00805f9b34fb';
const CTRL_UUID = '0000bbcd-0000-1000-8000-00805f9b34fb';  // write without response
const EVTS_UUID = '0000bbce-0000-1000-8000-00805f9b34fb';  // notify (gyro data)

class BLEWristbandService {
  manager: BleManager;
  device: Device | null = null;
  controlChar: Characteristic | null = null;
  eventChar: Characteristic | null = null;
  notifySub: { remove?: () => void } | null = null;

  constructor() {
    this.manager = new BleManager();
  }

  // Request permissions (Android 12+)
  async requestBlePermissions() {
    if (Platform.OS === 'android' && Platform.Version >= 31) {
      const result = await PermissionsAndroid.requestMultiple([
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
        PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
        //PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      ]);
      console.log('BLE Permissions:', result);
    }
  }

  // Connect to the wristband
  async connect(): Promise<void> {
    await this.requestBlePermissions();
    return new Promise((resolve, reject) => {
      const timeout = setTimeout(() => {
        this.manager.stopDeviceScan();
        reject(new Error('Wristband not found'));
      }, 8000);

      this.manager.startDeviceScan(null, null, async (error, d) => {
        if (error) {
          clearTimeout(timeout);
          this.manager.stopDeviceScan();
          return reject(error);
        }

        if (d?.name === WRISTBAND_NAME) {
          console.log('Found wristband:', d.name);
          clearTimeout(timeout);
          this.manager.stopDeviceScan();

          try {
            await d.connect();
            await d.discoverAllServicesAndCharacteristics();
            this.device = d;

            const chars = await d.characteristicsForService(SERVICE_UUID);
            this.controlChar = chars.find(c => c.uuid.toLowerCase() === CTRL_UUID) || null;
            console.log('Connected to JoyLabWristband!');
            resolve();
          } catch (e) {
            reject(e);
          }
        }
      });
    });
  }

  // Send control frame (e.g., start/stop IMU streaming)
  async sendControlFrame(bytes: number[]) {
    if (!this.controlChar) throw new Error('Not connected to wristband');
    const frame = Buffer.from(bytes).toString('base64');
    await this.controlChar.writeWithoutResponse(frame);
  }

  // Subscribe to IMU notifications (gyro data)
  async enableIMUNotifications(callback: (gx: number, gy: number, gz: number) => void) {
    if (!this.device) throw new Error('Not connected');
    const chars = await this.device.characteristicsForService(SERVICE_UUID);
    const evtChar = chars.find(c => c.uuid.toLowerCase() === EVTS_UUID);
    if (!evtChar) throw new Error('IMU events characteristic not found');

    this.eventChar = evtChar;

    // Subscribe for notifications
    this.notifySub = evtChar.monitor((error, characteristic) => {
      if (error) {
        console.error('BLE notify error:', error);
        return;
      }

      if (characteristic?.value) {
        const data = Buffer.from(characteristic.value, 'base64');

        // Expecting 12 bytes: 3 floats (gx, gy, gz)
        if (data.length >= 12) {
          const gx = data.readFloatLE(0);
          const gy = data.readFloatLE(4);
          const gz = data.readFloatLE(8);
          callback(gx, gy, gz);
        } else {
          console.warn('Unexpected IMU data length:', data.length);
        }
      }
    });

    console.log('IMU notifications enabled for JoyLabWristband');
  }

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

  // Stop IMU notifications
  disableIMUNotifications() {
    if (this.notifySub) {
      this.notifySub.remove?.();
      this.notifySub = null;
      console.log('IMU notifications disabled');
    }
  }

  // Cleanup connection
  async disconnect() {
    if (this.device) {
      await this.device.cancelConnection();
      console.log('Disconnected from JoyLabWristband');
      this.device = null;
    }
  }
}

export const bleWristbandService = new BLEWristbandService();
