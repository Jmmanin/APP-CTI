using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class finger4Seg1 : MonoBehaviour {
	public finger4Seg2 seg;

	// Use this for initialization
	void Start () {
		seg = FindObjectOfType(typeof(finger4Seg2)) as finger4Seg2;
	}
	
	// Update is called once per frame
	void Update () {
		
	}

	public void Bend (float x, Vector3 palm) {
		float scaledAmount = -0.001190476f + 1.300992f * x + 2.0625f * x * x - 6.527778f * x * x * x + 4.166667f * x * x * x * x;

		float first = scaledAmount * 88;
		transform.rotation = Quaternion.Euler(new Vector3(0, 0, palm.x + 90 + first));
		seg.Bend (first, x, palm);
	}
}
