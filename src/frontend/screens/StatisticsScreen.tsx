import React, { useState, useEffect } from 'react';
import { View, Text, StyleSheet, FlatList, ActivityIndicator } from 'react-native';
import { supabase } from "../../backend/app/services/supabase";
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

type Stat = {
  id: number;
  hits: number;
  attempts: number;
  created_at: string;
  mode: string | null;
  // user_id?: string; // add if you have this column
};



export default function StatisticsScreen() {
  const [stats, setStats] = useState<Stat[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  console.log("📊 StatisticsScreen loaded!");

  useEffect(() => {
    const fetchStats = async () => {
      try {
        setLoading(true);
        const { data, error } = await supabase
          .from('stats')
          .select('*')
          .order('created_at', { ascending: false });
        if (error) {
          setError(error.message);
        } else {
          setStats(data || []);
        }
      } catch (e: any) {
        setError(e.message);
      } finally {
        setLoading(false);
      }
    };

    fetchStats();
  }, []);

  if (loading) {
    return (
      <View style={styles.container}>
        <ActivityIndicator size="large" color="#0000ff" />
      </View>
    );
  }

  if (error) {
    return (
      <View style={styles.container}>
        <Text>Error: {error}</Text>
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <Text>Stats</Text>
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
