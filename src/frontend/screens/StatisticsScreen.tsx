import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

export default function StatisticsScreen() {
  console.log("📊 StatisticsScreen loaded!");
  return (
    <View style={styles.container}>
      <Text>Statss</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
