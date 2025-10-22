> **Note**: Make sure you have completed the [Set Up Your Environment](https://reactnative.dev/docs/set-up-your-environment) guide before proceeding.

# Ensure node.js is installed properly
Can verify using the following command: 
```sh
node -v

npm -v
```

# Proper configuration of packages
```sh
npm install
```

# Running the app on an android emulator
Ensure in correct directory within terminal before running through following steps

## 1. Start the emulator
```sh
emulator -list-avds

emulator -avd <device name>
```
Wait until Android home screen shows.

## 2. Start Metro Bundler
In one terminal:
```sh
npx react-native start
```

## 3. Build and install the application
In a second terminal:
```sh
npx react-native run-android
```

# Running the app on andorid device
## 1. Ensure phone is in developer mode
Settings
About Phone
Tap 'Build number' 7 times

## 2. Connect phone to laptop with code

## 3. Start Metro Bundler
In one terminal:
```sh
npx react-native start
```

## 4. Build and install application
In a second terminal:

```sh
npx react-native run-android
```
